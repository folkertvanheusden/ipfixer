#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <time.h>

#include "buffer.h"
#include "db-common.h"


class db
{
private:

public:
	db();
	virtual ~db();

	virtual bool insert(const db_record_t & dr) = 0;
};

std::string pull_field_from_db_record_t(db_record_t & data, const std::string & key, const std::string & default_);
