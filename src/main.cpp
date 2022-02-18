#include <atomic>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#include "config.h"
#include "db.h"
#include "db-influxdb.h"
#include "db-mongodb.h"
#include "db-mysql.h"
#include "db-postgres.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "netflow-v5.h"
#include "netflow-v9.h"
#include "yaml-helpers.h"


std::atomic_bool stop_flag { false };

void sigh(int sig)
{
	stop_flag = true;

	dolog(ll_info, "Terminating...");
}

db_field_mappings_t retrieve_mappings(const YAML::Node & cfg_storage)
{
	// mappings from iana names to database field names
	// also selects wether the target field is a json-blob
	db_field_mappings_t dfm;

	YAML::Node cfg_map = cfg_storage["map"];
	for(YAML::const_iterator it = cfg_map.begin(); it != cfg_map.end(); it++) {
		const YAML::Node node    = it->as<YAML::Node>();

		std::string      iana    = yaml_get_string(node, "iana",    "original name of the field as registered at IANA");
		std::string      host    = yaml_get_string(node, "field",   "name of field in the database");
		bool             is_json = yaml_get_bool  (node, "is-json", "sets if the field is a json blob or not");

		dfm.mappings.insert({ iana, { host, is_json } });
	}

	dfm.unmapped_fields     = yaml_get_string(cfg_storage, "unmapped-fields", "in what JSON blob to store unmapped fields, leave empty to skip");

	return dfm;
}

db_timeseries_aggregations_t retrieve_aggregations(const YAML::Node & cfg_storage)
{
	db_timeseries_aggregations_t dta;

	// AGGREGATE octetDeltaCount FOR ingressInterface=0,ipVersion=6 TO nurdspace.edgerouter.ipv6 INTERVAL=60s

	YAML::Node cfg_map = cfg_storage["aggregations"];
	for(YAML::const_iterator it = cfg_map.begin(); it != cfg_map.end(); it++) {
		const YAML::Node node    = it->as<YAML::Node>();

		db_aggregation_t da;

		std::string      aggregation_field = yaml_get_string(node, "field",    "field to aggregate");
		int              emit_interval     = yaml_get_int   (node, "interval", "emit interval (in seconds)");
		std::string      publish_topic     = yaml_get_string(node, "topic",    "topic to publish values under");
		std::string      type              = yaml_get_string(node, "type",     "what to do with the value: sum, average or count");

		if (type != "sum" && type != "average")
			error_exit(false, "retrieve_aggregations: type \"%s\" not recognized", type.c_str());

		da.emit_interval     = emit_interval;
		da.publish_topic     = publish_topic;
		da.type              = type;

		YAML::Node       rules             = node["rules"];
		for(YAML::const_iterator it = rules.begin(); it != rules.end(); it++) {
			const YAML::Node rule_node = it->as<YAML::Node>();

			std::string      match_key = yaml_get_string(rule_node, "match-key", "field to check");
			std::string      match_val = yaml_get_string(rule_node, "match-val", "value to check for");

			da.rules.push_back({ match_key, match_val });
		}

		dta.aggregations.insert({ aggregation_field, da });
	}

	return dta;
}

void help()
{
	printf("-c file  load configuration from 'file'\n");
	printf("-f       fork into the background\n");
	printf("-h       this help\n");
}

int main(int argc, char *argv[])
{
	setlog("ipfixer.log", ll_debug, ll_debug);

	signal(SIGINT, sigh);

	std::string cfg_file = "ipfixer.yaml";
	bool        do_fork  = false;

	int c = -1;
	while((c = getopt(argc, argv, "c:fh")) != -1) {
		if (c == 'c')
			cfg_file = optarg;
		else if (c == 'f')
			do_fork = true;
		else if (c == 'h') {
			help();
			return 0;
		}
		else {
			help();
			return 1;
		}
	}

	db *db = nullptr;

	YAML::Node config;

	try {
		dolog(ll_debug, "Loading configuration file: \"%s\"", cfg_file.c_str());

		config = YAML::LoadFile(cfg_file);
	}
	catch(const YAML::BadFile & bf) {
		error_exit(true, "Failed loading configuration file \"%s\" - does it exist?", cfg_file.c_str());
	}

	try {
		// configure logfile
		YAML::Node cfg_logging = config["logging"];
		const std::string logfile = yaml_get_string(cfg_logging, "file", "log-file to log to");

		log_level_t ll_file = str_to_ll(yaml_get_string(cfg_logging, "loglevel-files", "log-level for log-file"));
		log_level_t ll_screen = str_to_ll(yaml_get_string(cfg_logging, "loglevel-screen", "log-level for screen output"));

		setlog(logfile.c_str(), ll_file, ll_screen);

		// select target
		YAML::Node cfg_storage = config["storage"];
		std::string storage_type = yaml_get_string(cfg_storage, "type", "Database type to write to: 'influxdb', 'mariadb'/'mysql', 'mongodb' or 'postgres'");

#if LIBMONGOCXX_FOUND == 1
		if (storage_type == "mongodb") {
			std::string mongodb_uri = yaml_get_string(cfg_storage, "uri", "MongoDB URI");
			std::string mongodb_db  = yaml_get_string(cfg_storage, "db", "MongoDB database to write to");
			std::string mongodb_collection = yaml_get_string(cfg_storage, "collection", "collection to write to");

			db_field_mappings_t dfm = retrieve_mappings(cfg_storage);

			db = new db_mongodb(mongodb_uri, mongodb_db, mongodb_collection, dfm);
		}
		else
#endif
#if POSTGRES_FOUND == 1
		if (storage_type == "postgres") {
			std::string         connection_info = yaml_get_string(cfg_storage, "connection-info", "Postgres connection info string");

			db_field_mappings_t dfm = retrieve_mappings(cfg_storage);

			db = new db_postgres(connection_info, dfm);
		}
		else
#endif
#if MARIADB_FOUND == 1
		if (storage_type == "mariadb" || storage_type == "mysql") {
			std::string         host   = yaml_get_string(cfg_storage, "host", "MariaDB host to connect to");
			std::string         user   = yaml_get_string(cfg_storage, "user", "username to authenticate with");
			std::string         pass   = yaml_get_string(cfg_storage, "pass", "passwor to authenticate with");
			std::string         dbname = yaml_get_string(cfg_storage, "db",   "database to write to");

			db_field_mappings_t dfm    = retrieve_mappings(cfg_storage);

			db = new db_mysql(host, user, pass, dbname, dfm);
		}
		else
#endif
		if (storage_type == "influxdb") {
			std::string host   = yaml_get_string(cfg_storage, "host", "InfluxDB host to connect to");
			int         port   = yaml_get_int   (cfg_storage, "port", "InfluxDB port to connect to");

			db_timeseries_aggregations_t dta = retrieve_aggregations(cfg_storage);

			db = new db_influxdb(host, port, dta);
		}
		else
		{
			error_exit(false, "Database \"%s\" not supported/understood", storage_type.c_str());
		}

		db->init_database();

		// port to listen on
		int         listen_port = yaml_get_int   (config, "listen-port", "UDP port to listen on");
		std::string protocol    = yaml_get_string(config, "protocol", "protocol to accept; \"ipfix\", \"v5\" or \"v9\" (v5/v9 are NetFlow)");

		ipfix *i = nullptr;

		if (protocol == "ipfix")
			i = new ipfix();
		else if (protocol == "v9")
			i = new netflow_v9();
		else if (protocol == "v5")
			i = new netflow_v5();
		else
			error_exit(false, "Protocol \"%s\" not supported/understood", protocol.c_str());

		dolog(ll_debug, "main: will listen on port %d", listen_port);

		int fd = create_udp_listen_socket(listen_port);
		if (fd == -1)
			error_exit(true, "Cannot create UDP listening socket on port %d", listen_port);

		if (do_fork) {
			if (daemon(-1, -1) == -1)
				error_exit(true, "main: can't become daemon process");
		}

		struct pollfd fds[] { { fd, POLLIN, 0 } };

		while(!stop_flag) {
			int rcp = poll(fds, 1, 500);

			if (rcp == 0)  // timeout
				continue;

			if (rcp == -1) {
				dolog(ll_error, "main: problem polling for UDP packets: %s", strerror(errno));
				break;
			}

			uint8_t buffer[65527] { 0 };
			int rrc = recv(fd, buffer, sizeof buffer, 0);

			if (rrc == -1) {
				dolog(ll_error, "main: problem receiving UDP packet: %s", strerror(errno));
				continue;
			}

			if (i->process_packet(buffer, rrc, db) == false)
				dolog(ll_error, "main: problem processing ipfix packet");
		}

		close(fd);

		delete i;

		delete db;
	}
	catch(const std::out_of_range & e) {
		dolog(ll_error, "main: 'out of range' exception (%s); internal error", e.what());
	}
	catch(const std::string & e) {
		dolog(ll_error, "main: an error occured: \"%s\"", e.c_str());
	}
	catch(const YAML::Exception & e) {
		dolog(ll_error, "During processing of the configuration (yaml-) file, the following error occured: %s", e.what());
	}

	return 0;
}
