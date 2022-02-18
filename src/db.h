#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <time.h>

#include "buffer.h"
#include "db-common.h"


class db
{
protected:
	const db_field_mappings_t field_mappings;

	std::string         timestamp_type { "" };
	std::string         json_type      { "" };

	std::string         pull_field_from_db_record_t(db_record_t & data, const std::string & key, const std::string & default_);

	virtual std::string data_type_to_db_type(const data_type_t dt) = 0;

	virtual std::string escape_string(const std::string & in) = 0;
	virtual bool        execute_query(const std::string & q) = 0;
	virtual bool        commit() = 0;

public:
	db(const db_field_mappings_t & field_mappings);
	virtual ~db();

	virtual void init_database();

	virtual bool insert(const db_record_t & dr);
};
