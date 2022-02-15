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
#include <yaml-cpp/yaml.h>

#include "db.h"
#include "db-mongodb.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "netflow-v9.h"
#include "yaml-helpers.h"


std::atomic_bool stop_flag { false };

void sigh(int sig)
{
	stop_flag = true;

	dolog(ll_info, "Terminating...");
}

void help()
{
	printf("-c file  load configuration from 'file'\n");
	printf("-h       this help\n");
}

int main(int argc, char *argv[])
{
	setlog("ipfixer.log", ll_debug, ll_debug);

	signal(SIGINT, sigh);

	std::string cfg_file = "ipfixer.yaml";

	int c = -1;
	while((c = getopt(argc, argv, "c:h")) != -1) {
		if (c == 'c')
			cfg_file = optarg;
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
		std::string storage_type = yaml_get_string(cfg_storage, "type", "Database type to write to - currently only mongodb");
		if (storage_type != "mongodb")
			error_exit(false, "Currently only \"MongoDB\" is supported");

		std::string storage_uri = yaml_get_string(cfg_storage, "uri", "Database URI");

		if (storage_type == "mongodb") {
			std::string mongodb_db = yaml_get_string(cfg_storage, "db", "MongoDB database to write to");
			std::string mongodb_collection = yaml_get_string(cfg_storage, "collection", "collection to write to");

			db = new db_mongodb(storage_uri, mongodb_db, mongodb_collection);
		}

		// port to listen on
		int         listen_port = yaml_get_int   (config, "listen-port", "UDP port to listen on");
		std::string protocol    = yaml_get_string(config, "protocol", "protocol to accept; \"ipfix\" or \"v9\" (=netflow)");

		ipfix *i = nullptr;

		if (protocol == "ipfix")
			i = new ipfix();
		else if (protocol == "v9")
			i = new netflow_v9();
		else
			error_exit(false, "Protocol \"%s\" not supported/understood", protocol.c_str());

		dolog(ll_debug, "main: will listen on port %d", listen_port);

		int fd = create_udp_listen_socket(listen_port);

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

	return 0;
}
