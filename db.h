#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <time.h>


typedef struct
{
	time_t   export_time;
	uint32_t sequence_number;
	uint32_t observation_domain_id;

	// key, value
	std::map<std::string, std::string> data;
} db_record_t;

class db
{
private:

public:
	db();
	virtual ~db();

	virtual bool insert(const db_record_t & dr) = 0;
};
