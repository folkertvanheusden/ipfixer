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


void exec_query(MYSQL *const handle, const std::string & query)
{
	// printf("%s\n", query.c_str());

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

std::string escape_string(MYSQL *const handle, const std::string & in)
{
	size_t  len = in.size();
	char   *temp = reinterpret_cast<char *>(calloc(1, len * 2 + 1));
	
	if (!temp)
		error_exit(true, "escape_string: failed to allocate memory");

	if (mysql_real_escape_string(handle, temp, in.c_str(), len) == (unsigned long)-1)
		error_exit("escape_string: mysql_real_escape_string on \"%s\" failed: %s", in.c_str(), mysql_error(handle));

	std::string out = temp;

	free(temp);

	return out;
}

db_mysql::db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database, const db_field_mappings_t & field_mappings) :
	field_mappings(field_mappings)
{
	handle = mysql_init(nullptr);
	if (!handle)
		error_exit(false, "db_mysql: failed to initialize MySQL library");

        if (mysql_real_connect(handle, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0) == 0)
		error_exit(false, "db_mysql: failed to connect to MySQL database, season: %s", mysql_error(handle));

	std::string query = "CREATE TABLE IF NOT EXISTS records(ts DATETIME NOT NULL";

	// json-fields must be created once
	std::set<std::string> json_fields;

	for(auto & mapping : field_mappings.mappings) {
		std::string type;
		bool        add = true;

		if (mapping.second.target_is_json) {
			type = "JSON";

			if (json_fields.find(mapping.second.target_name) != json_fields.end())
				add = false;
			else
				json_fields.insert(mapping.second.target_name);
		}
		else {
			auto data_type = field_lookup.get_data_type(mapping.first);

			if (data_type.has_value() == false)
				error_exit(false, "db_mysql: field with name \"%s\" is not known", mapping.second.target_name.c_str());

			auto dt = data_type.value();

			if (dt == dt_octetArray)
				type = "BLOB";
			else if (dt == dt_unsigned8)
				type = "TINYINT UNSIGNED";
			else if (dt == dt_unsigned16)
				type = "SMALLINT UNSIGNED";
			else if (dt == dt_unsigned32)
				type = "INT UNSIGNED";
			else if (dt == dt_unsigned64)
				type = "BIGINT UNSIGNED";
			else if (dt == dt_signed8)
				type = "TINYINT";
			else if (dt == dt_signed16)
				type = "SMALLINT";
			else if (dt == dt_signed32)
				type = "INT";
			else if (dt == dt_signed64)
				type = "BIGINT";
			else if (dt == dt_float32)
				type = "FLOAT";
			else if (dt == dt_float64)
				type = "DOUBLE";
			else if (dt == dt_boolean)
				type = "BOOLEAN";
			else if (dt == dt_macAddress)
				type = "CHAR(17)";
			else if (dt == dt_string)
				type = "VARCHAR(256)";
			else if (dt == dt_dateTimeSeconds)
				type = "INT UNSIGNED";
			else if (dt == dt_dateTimeMilliseconds)
				type = "BIGINT UNSIGNED";
			else if (dt == dt_dateTimeMicroseconds)
				type = "BIGINT UNSIGNED";
			else if (dt == dt_dateTimeNanoseconds)
				type = "BIGINT UNSIGNED";
			else if (dt == dt_ipv4Address)
				type = "VARCHAR(15)";
			else if (dt == dt_ipv6Address)
				type = "VARCHAR(48)";
			// else if (dt == dt_basicList)
			// else if (dt == dt_subTemplateList)
			// else if (dt == dt_subTemplateMultiList)
			else {
				error_exit(false, "db_mysql: field \"%s\" has a data-type (%d) that is not supported (yet)", mapping.second.target_name.c_str(), dt);
			}
		}

		if (add)
			query += ", " + mapping.second.target_name + " " + type + " NOT NULL";
	}

	if (field_mappings.unmapped_fields.empty() == false)
		query += ", " + field_mappings.unmapped_fields + " JSON NOT NULL";

	query += ")";

	exec_query(handle, query);
}

db_mysql::~db_mysql()
{
}

bool db_mysql::insert(const db_record_t & dr)
{
	try {
		db_record_t work = dr;

		std::string query = "INSERT INTO records(ts";

		std::map<std::string, json_t *> json_values;

		for(auto & mapping : field_mappings.mappings) {
			if (json_values.find(mapping.second.target_name) == json_values.end()) {
				query += ", " + mapping.second.target_name;

				json_values.insert({ mapping.second.target_name, json_object() });
			}
		}

		if (field_mappings.unmapped_fields.empty() == false)
			query += ", " + field_mappings.unmapped_fields;

		query += ") VALUES(NOW()";

		// first collect all json-fields
		for(auto & mapping : field_mappings.mappings) {
			if (mapping.second.target_is_json) {
				auto it = json_values.find(mapping.second.target_name);
				// TODO check if it valid

				auto value = pull_field_from_db_record_t(work, mapping.first, "0");
				// TODO check if it valid

				// map IANA name in JSON-object to value
				json_object_set(it->second, mapping.first.c_str(), json_string(value.c_str()));
			}
		}

		std::set<std::string> json_fields;

		// then add all fields to query
		for(auto & mapping : field_mappings.mappings) {
			std::string value;
			bool        add = false;

			if (mapping.second.target_is_json) {
				if (json_fields.find(mapping.second.target_name) == json_fields.end()) {
					json_fields.insert(mapping.second.target_name);

					auto it = json_values.find(mapping.second.target_name);
					// TODO check if it valid

					char *misc_str = json_dumps(it->second, 0);

					value = misc_str;

					free(misc_str);

					add = true;
				}
			}
			else {
				value = pull_field_from_db_record_t(work, mapping.first, "0");

				add = true;
			}

			if (add)
				query += ", '" + escape_string(handle, value) + "'";
		}

		// left-over fields
		json_t *unmapped_fields = json_object();
		if (field_mappings.unmapped_fields.empty() == false) {
			// get
			while(work.data.empty() == false) {
				std::string field = work.data.begin()->first;

				auto value = pull_field_from_db_record_t(work, field, "0");
				// TODO check if it valid

				json_object_set(unmapped_fields, field.c_str(), json_string(value.c_str()));
			}

			// add to query
			char *unmapped_fields_str = json_dumps(unmapped_fields, 0);

			std::string value = unmapped_fields_str;

			free(unmapped_fields_str);

			query += ", '" + escape_string(handle, value) + "'";
		}

		query += ")";

		start_transaction(handle);
		exec_query(handle, query);
		commit_transaction(handle);

		for(auto element : json_values)
			json_decref(element.second);

		json_decref(unmapped_fields);
	}
	catch(const std::string & s) {
		dolog(ll_warning, "db_mariadb::insert: problem during record insertion: %s", s.c_str());

		return false;
	}

	return true;
}
#endif
