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
	const db_field_mappings_t  field_mappings;

	std::optional<std::string> pull_field_from_db_record_t(db_record_t & data, const std::string & key);

public:
	db(const db_field_mappings_t & field_mappings);
	virtual ~db();

	virtual void init_database() = 0;

	virtual bool insert(const db_record_t & dr) = 0;
};
