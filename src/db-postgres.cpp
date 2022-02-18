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


db_postgres::db_postgres(const std::string & connection_info, const db_field_mappings_t & field_mappings) : db(field_mappings)
{
	connection = new pqxx::connection(connection_info);

	timestamp_type = "TIMESTAMP";
	json_type      = "JSONB";
}

db_postgres::~db_postgres()
{
	delete connection;
}

std::string db_postgres::escape_string(const std::string & in)
{
	return connection->esc(in);
}

bool db_postgres::execute_query(const std::string & query)
{
	pqxx::work work { *connection };
	work.exec(query);

	work.commit();  // TODO: move to 'commit'-method

	return true;
}

bool db_postgres::commit()
{
	return true;
}

std::string db_postgres::data_type_to_db_type(const data_type_t dt) 
{
	if (dt == dt_octetArray)
		return "BLOB";
	else if (dt == dt_unsigned8)
		return "SMALLINT";
	else if (dt == dt_unsigned16)
		return "INT";
	else if (dt == dt_unsigned32)
		return "BIGINT";
	else if (dt == dt_unsigned64)
		return "BIGINT";
	else if (dt == dt_signed8)
		return "SMALLINT";
	else if (dt == dt_signed16)
		return "SMALLINT";
	else if (dt == dt_signed32)
		return "INT";
	else if (dt == dt_signed64)
		return "BIGINT";
	else if (dt == dt_float32)
		return "FLOAT";
	else if (dt == dt_float64)
		return "DOUBLE";
	else if (dt == dt_boolean)
		return "BOOLEAN";
	else if (dt == dt_macAddress)
		return "CHAR(17)";
	else if (dt == dt_string)
		return "VARCHAR(256)";
	else if (dt == dt_dateTimeSeconds)
		return "INT";
	else if (dt == dt_dateTimeMilliseconds)
		return "BIGINT";
	else if (dt == dt_dateTimeMicroseconds)
		return "BIGINT";
	else if (dt == dt_dateTimeNanoseconds)
		return "BIGINT";
	else if (dt == dt_ipv4Address)
		return "INET";
	else if (dt == dt_ipv6Address)
		return "INET";
	// else if (dt == dt_basicList)
	// else if (dt == dt_subTemplateList)
	// else if (dt == dt_subTemplateMultiList)

	error_exit(false, "db_postgres::data_type_to_db_type: data-type %d is not supported (yet)", dt);
}
#endif
