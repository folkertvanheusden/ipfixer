#include <jansson.h>
#include <map>
#include <set>
#include <string>

#include "db.h"
#include "error.h"
#include "logging.h"
#include "ipfix.h"
#include "str.h"


db::db(const db_field_mappings_t & field_mappings) : field_mappings(field_mappings)
{
}

db::~db()
{
}

void db::init_database()
{
	std::string query = "CREATE TABLE IF NOT EXISTS records(ts " + timestamp_type + " NOT NULL";

	// json-fields must be created once
	std::set<std::string> json_fields;

	for(auto & mapping : field_mappings.mappings) {
		std::string type;
		bool        add = true;

		if (mapping.second.target_is_json) {
			type = json_type;

			if (json_fields.find(mapping.second.target_name) != json_fields.end())
				add = false;
			else
				json_fields.insert(mapping.second.target_name);
		}
		else {
			auto data_type = field_lookup.get_data_type(mapping.first);

			if (data_type.has_value() == false)
				error_exit(false, "db::init_database: field with name \"%s\" is not known", mapping.second.target_name.c_str());

			type = data_type_to_db_type(data_type.value());
		}

		if (add)
			query += ", " + mapping.second.target_name + " " + type + " NOT NULL";
	}

	if (field_mappings.unmapped_fields.empty() == false)
		query += ", " + field_mappings.unmapped_fields + " " + json_type + " NOT NULL";

	query += ")";

	execute_query(query);
}

// get & erase!
std::optional<std::string> db::pull_field_from_db_record_t(db_record_t & data, const std::string & key)
{
	auto it = data.data.find(key);

	if (it == data.data.end())
		return { };

	auto value = ipfix::data_to_str(it->second.dt, it->second.len, it->second.b);

	if (value.has_value() == false) {
		dolog(ll_info, "pull_field_from_db_record_t: cannot convert \"%s\" (data-type %d) to string", key.c_str(), it->second.dt);

		return { };
	}

	data.data.erase(it);

	return value.value();
}

bool db::insert(const db_record_t & dr)
{
	try {
		bool        fail = false;
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
				if (it == json_values.end()) {
					dolog(ll_info, "db::insert: json_values no longer has \"%s\"", mapping.second.target_name.c_str());

					fail = true;
					break;
				}

				std::string value;
				auto temp = pull_field_from_db_record_t(work, mapping.first);
				if (temp.has_value() == false) {
					dolog(ll_info, "db::insert: data for \"%s\" missing (json)", mapping.first.c_str());
					value = "0";
				}
				else {
					value = temp.value();
				}

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
					if (it == json_values.end()) {
						dolog(ll_info, "db::insert: json_values no longer has \"%s\"", mapping.second.target_name.c_str());

						fail = true;
						break;
					}

					char *misc_str = json_dumps(it->second, 0);

					value = misc_str;

					free(misc_str);

					add = true;
				}
			}
			else {
				auto temp = pull_field_from_db_record_t(work, mapping.first);
				if (temp.has_value() == false) {
					dolog(ll_info, "db::insert: data for \"%s\" missing (non-json)", mapping.first.c_str());
					value = "0";
				}
				else {
					value = temp.value();
				}

				add = true;
			}

			if (add)
				query += ", '" + escape_string(value) + "'";
		}

		// left-over fields
		json_t *unmapped_fields = json_object();
		if (field_mappings.unmapped_fields.empty() == false && !fail) {
			// get
			while(work.data.empty() == false) {
				std::string field = work.data.begin()->first;

				auto value = pull_field_from_db_record_t(work, field);
				if (value.has_value() == false) {
					dolog(ll_info, "db::insert: data for \"%s\" missing (unmapped)", field.c_str());

					fail = true;
					break;
				}

				json_object_set(unmapped_fields, field.c_str(), json_string(value.value().c_str()));
			}

			// add to query
			char *unmapped_fields_str = json_dumps(unmapped_fields, 0);

			std::string value = unmapped_fields_str;

			free(unmapped_fields_str);

			query += ", '" + escape_string(value) + "'";
		}

		query += ")";

		if (!fail) {
			execute_query(query);
			commit();
		}

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
