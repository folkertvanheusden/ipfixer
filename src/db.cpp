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

// get & erase!
std::optional<std::string> db::pull_field_from_db_record_t(db_record_t & data, const std::string & key)
{
	auto it = data.data.find(key);

	if (it == data.data.end())
		return { };

	auto value = ipfix::data_to_str(it->second.dt, it->second.len, it->second.b);

	if (value.has_value() == false) {
		dolog(ll_info, "db::pull_field_from_db_record_t: cannot convert \"%s\" (data-type %d) to string", key.c_str(), it->second.dt);

		return { };
	}

	data.data.erase(it);

	return value.value();
}
