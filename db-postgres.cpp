#include "config.h"
#if POSTGRES_FOUND == 1
#if JANSSON_FOUND != 1
#error libjansson is required for postgresql
#endif
#include <jansson.h>
#include <stdexcept>
#include <pqxx/pqxx>

#include "db-postgres.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"
#include "str.h"


db_postgres::db_postgres(const std::string & connection_info)
{
	connection = new pqxx::connection(connection_info);

	pqxx::work work { *connection };

	work.exec(
		"CREATE TABLE IF NOT EXISTS records("
			"ts TIMESTAMP WITHOUT TIME ZONE,"
			"protocolIdentifier SMALLINT NOT NULL,"  // should be 8 bit unsigned
			"sourceIPAddress INET NOT NULL,"
			"destinationIPAddress INET NOT NULL,"
			"sourceTransportPort INTEGER NOT NULL,"  // should be 16 bit unsigned
			"destinationTransportPort INTEGER NOT NULL,"  // should be 16 bit unsigned
			"ipClassOfService SMALLINT NOT NULL,"  // should be 8 bit unsigned
			"ingressInterface INTEGER NOT NULL,"  // should be unsigned
			"egressInterface INTEGER NOT NULL,"  // should be unsigned
			"octetDeltaCount BIGINT NOT NULL,"  // should be unsigned
			"packetDeltaCount BIGINT NOT NULL,"  // should be unsigned
			"flowEndSysUpTime INTEGER NOT NULL,"  // should be unsigned
			"flowStartSysUpTime INTEGER NOT NULL,"  // should be unsigned
			"tcpControlBits INTEGER NOT NULL,"  // should be 16 bit unsigned
			"miscellaneous JSONB"
		")");

	work.exec(
		"CREATE INDEX IF NOT EXISTS ts_index ON records(ts)");

	work.commit();
}

db_postgres::~db_postgres()
{
	delete connection;
}

bool db_postgres::insert(const db_record_t & dr)
{
	try {
		db_record_t work = dr;

		std::string protocolIdentifier   = pull_field_from_db_record_t(work, "protocolIdentifier", "0");
		std::string sourceIPAddress      = pull_field_from_db_record_t(work, "sourceIPv6Address", pull_field_from_db_record_t(work, "sourceIPv4Address", "0.0.0.0"));
		std::string destinationIPAddress = pull_field_from_db_record_t(work, "destinationIPv6Address", pull_field_from_db_record_t(work, "destinationIPv4Address", "0.0.0.0"));
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
				dolog(ll_warning, "db_postgres::insert: cannot convert \"%s\" (data-type %d) to string", element.first.c_str(), element.second.dt);

				return false;
			}

			json_object_set(obj, element.first.c_str(), json_string(value.value().c_str()));
		}

		char *misc_str = json_dumps(obj, 0);

		std::string miscellaneous = misc_str;

		free(misc_str);

		pqxx::work work_db { *connection };

		std::string query = std::string("INSERT INTO records") +
				"(ts, protocolIdentifier, sourceIPAddress, destinationIPAddress, sourceTransportPort, destinationTransportPort, ipClassOfService, " +
				" ingressInterface, egressInterface, octetDeltaCount, packetDeltaCount, flowEndSysUpTime, flowStartSysUpTime, tcpControlBits, " +
				" miscellaneous) " +
				"VALUES(" +
					"NOW()," +
					protocolIdentifier + "," +
					"'" + sourceIPAddress + "'," +
					"'" + destinationIPAddress + "'," +
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
					"'" + work_db.esc(miscellaneous) + "'" + 
				")";

		work_db.exec(query);

		work_db.commit();
	}
	catch(const std::string & s) {
		dolog(ll_warning, "db_postgres::insert: problem during record insertion: %s", s.c_str());

		return false;
	}

	return true;
}
#endif
