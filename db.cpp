#include <string>

#include "db.h"
#include "ipfix.h"
#include "str.h"


db::db()
{
}

db::~db()
{
}

// get & erase!
std::string pull_field_from_db_record_t(db_record_t & data, const std::string & key, const std::string & default_)
{
	auto it = data.data.find(key);

	if (it == data.data.end())
		return default_;

	auto value = ipfix::data_to_str(it->second.dt, it->second.len, it->second.b);

	if (value.has_value() == false)
		throw myformat("pull_field_from_db_record_t: cannot convert \"%s\" (data-type %d) to string", key.c_str(), it->second.dt);

	data.data.erase(it);

	return value.value();
}
