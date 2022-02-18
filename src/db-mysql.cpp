#include "config.h"
#if MARIADB_FOUND == 1
#if JANSSON_FOUND != 1
#error libjansson is required for mariadb
#endif
#include <jansson.h>
#include <map>
#include <set>
#include <mariadb/mysql.h>

#include "db-mysql.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"


db_mysql::db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database, const db_field_mappings_t & field_mappings) : db_sql(field_mappings)
{
	handle = mysql_init(nullptr);
	if (!handle)
		error_exit(false, "db_mysql: failed to initialize MySQL library");

        if (mysql_real_connect(handle, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0) == 0)
		error_exit(false, "db_mysql: failed to connect to MySQL database, season: %s", mysql_error(handle));

	timestamp_type = "DATETIME";
	json_type      = "JSON";
}

std::string db_mysql::escape_string(const std::string & in)
{
	size_t  len = in.size();
	char   *temp = reinterpret_cast<char *>(calloc(1, len * 2 + 1));

	if (!temp)
		error_exit(true, "db_mysql::escape_string: failed to allocate memory");

	if (mysql_real_escape_string(handle, temp, in.c_str(), len) == (unsigned long)-1)
		error_exit("db_mysql::escape_string: mysql_real_escape_string on \"%s\" failed: %s", in.c_str(), mysql_error(handle));

	std::string out = temp;

	free(temp);

	return out;
}

bool db_mysql::execute_query(const std::string & query)
{
	if (mysql_query(handle, query.c_str())) {
		dolog(ll_error, "db_mysql::execute_query: query \"%s\" failed, reason: %s", query.c_str(), mysql_error(handle));

		return false;
	}

	return true;
}

bool db_mysql::commit()
{
	return execute_query("commit");
}

std::string db_mysql::data_type_to_db_type(const data_type_t dt) 
{
	if (dt == dt_octetArray)
		return "BLOB";
	else if (dt == dt_unsigned8)
		return "TINYINT UNSIGNED";
	else if (dt == dt_unsigned16)
		return "SMALLINT UNSIGNED";
	else if (dt == dt_unsigned32)
		return "INT UNSIGNED";
	else if (dt == dt_unsigned64)
		return "BIGINT UNSIGNED";
	else if (dt == dt_signed8)
		return "TINYINT";
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
		return "INT UNSIGNED";
	else if (dt == dt_dateTimeMilliseconds)
		return "BIGINT UNSIGNED";
	else if (dt == dt_dateTimeMicroseconds)
		return "BIGINT UNSIGNED";
	else if (dt == dt_dateTimeNanoseconds)
		return "BIGINT UNSIGNED";
	else if (dt == dt_ipv4Address)
		return "VARCHAR(15)";
	else if (dt == dt_ipv6Address)
		return "VARCHAR(48)";
	// else if (dt == dt_basicList)
	// else if (dt == dt_subTemplateList)
	// else if (dt == dt_subTemplateMultiList)

	error_exit(false, "db_mysql::data_type_to_db_type: data-type %d is not supported (yet)", dt);
}

db_mysql::~db_mysql()
{
}
#endif
