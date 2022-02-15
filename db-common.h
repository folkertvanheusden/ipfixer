#include <map>
#include <string>
#include <stdint.h>

#include "buffer.h"
#include "ipfix-common.h"


typedef struct
{
	buffer      b;
	data_type_t dt;
	int         len;
} db_record_data_t;

typedef struct
{
	time_t   export_time;
	uint32_t sequence_number;
	uint32_t observation_domain_id;

	// key, value
	std::map<std::string, db_record_data_t> data;
} db_record_t;

