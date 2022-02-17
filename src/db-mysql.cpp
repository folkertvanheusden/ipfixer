#include "config.h"
#if MARIADB_FOUND == 1
#if JANSSON_FOUND != 1
#error libjansson is required for mariadb
#endif
#include <jansson.h>
#include <mariadb/mysql.h>

#include "db-mysql.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"


void exec_query(MYSQL *const handle, const std::string & query)
{
	if (mysql_query(handle, query.c_str()))
		error_exit(false, "exec_query: query \"%s\" failed, reason: %s", query.c_str(), mysql_error(handle));
}

void start_transaction(MYSQL *const handle)
{
	exec_query(handle, "begin");
}

void commit_transaction(MYSQL *const handle)
{
	exec_query(handle, "commit");
}

db_mysql::db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database)
{
	handle = mysql_init(nullptr);
	if (!handle)
		error_exit(false, "db_mysql: failed to initialize MySQL library");

        if (mysql_real_connect(handle, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0) == 0)
		error_exit(false, "db_mysql: failed to connect to MySQL database, season: %s", mysql_error(handle));

	// IP addresses are stored in the miscellaneous json structure
	// as they have multiple formats (IPv4 versus IPv6)
	exec_query(handle,
		"CREATE TABLE IF NOT EXISTS records("
			"ts DATETIME NOT NULL,"
			"protocolIdentifier TINYINT UNSIGNED NOT NULL,"
			"sourceTransportPort SMALLINT UNSIGNED NOT NULL,"
			"destinationTransportPort SMALLINT UNSIGNED NOT NULL,"
			"ipClassOfService TINYINT UNSIGNED NOT NULL,"
			"ingressInterface INTEGER UNSIGNED NOT NULL,"
			"egressInterface INTEGER UNSIGNED NOT NULL,"
			"octetDeltaCount BIGINT UNSIGNED NOT NULL,"
			"packetDeltaCount BIGINT UNSIGNED NOT NULL,"
			"flowEndSysUpTime INTEGER UNSIGNED NOT NULL,"
			"flowStartSysUpTime INTEGER UNSIGNED NOT NULL,"
			"tcpControlBits SMALLINT UNSIGNED NOT NULL,"
			"miscellaneous JSON)"
		);
}

db_mysql::~db_mysql()
{
}

bool db_mysql::insert(const db_record_t & dr)
{
	try {
		db_record_t work = dr;

		std::string protocolIdentifier   = pull_field_from_db_record_t(work, "protocolIdentifier", "0");
		std::string sourceTransportPort  = pull_field_from_db_record_t(work, "sourceTransportPort", "0");
		std::string destinationTransportPort  = pull_field_from_db_record_t(work, "destinationTransportPort", "0");
		std::string ipClassOfService     = pull_field_from_db_record_t(work, "ipClassOfService", "0");
		std::string ingressInterface     = pull_field_from_db_record_t(work, "ingressInterface", "0");
		std::string egressInterface      = pull_field_from_db_record_t(work, "egressInterface", "0");
		std::string octetDeltaCount      = pull_field_from_db_record_t(work, "octetDeltaCount", "0");
		std::string packetDeltaCount     = pull_field_from_db_record_t(work, "packetDeltaCount", "0");
		std::string flowEndSysUpTime     = pull_field_from_db_record_t(work, "flowEndSysUpTime", "0");
		std::string flowStartSysUpTime   = pull_field_from_db_record_t(work, "flowStartSysUpTime", "0");
		std::string tcpControlBits       = pull_field_from_db_record_t(work, "tcpControlBits", "0");

		json_t *obj = json_object();

		for(auto & element : work.data) {
			auto value = ipfix::data_to_str(element.second.dt, element.second.len, element.second.b);

			if (value.has_value() == false) {
				dolog(ll_warning, "db_mariadb::insert: cannot convert \"%s\" (data-type %d) to string", element.first.c_str(), element.second.dt);

				return false;
			}

			json_object_set(obj, element.first.c_str(), json_string(value.value().c_str()));
		}

		char *misc_str = json_dumps(obj, 0);

		std::string miscellaneous = misc_str;

		free(misc_str);

		std::string query = std::string("INSERT INTO records") +
				"(ts, protocolIdentifier, sourceTransportPort, destinationTransportPort, ipClassOfService, " +
				"ingressInterface, egressInterface, octetDeltaCount, packetDeltaCount, flowEndSysUpTime, " +
				"flowStartSysUpTime, tcpControlBits, miscellaneous) " +
				"VALUES(" +
					"NOW()," +
					protocolIdentifier + "," +
					sourceTransportPort + "," +
					destinationTransportPort + "," +
					ipClassOfService + "," +
					ingressInterface + "," +
					egressInterface + "," +
					octetDeltaCount + "," +
					packetDeltaCount + "," +
					flowEndSysUpTime + "," +
					flowStartSysUpTime + "," +
					tcpControlBits + "," +
					"'" + miscellaneous + "'" + 
				")";

		start_transaction(handle);
		exec_query(handle, query);
		commit_transaction(handle);
	}
	catch(const std::string & s) {
		dolog(ll_warning, "db_mariadb::insert: problem during record insertion: %s", s.c_str());

		return false;
	}

	return true;
}
#endif
