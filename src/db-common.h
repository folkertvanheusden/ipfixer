#pragma once
#include <map>
#include <mutex>
#include <string>
#include <stdint.h>
#include <thread>
#include <vector>

#include "buffer.h"
#include "ipfix-common.h"


// (no-)SQL
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

typedef struct
{
	std::string target_name;
	bool        target_is_json;
} db_field_t;

typedef struct
{
	std::map<std::string, db_field_t> mappings;

	std::string unmapped_fields;
} db_field_mappings_t;

// time series

typedef struct
{
	// thread that emits the samples every x seconds
	std::thread                  *th;
	int                           emit_interval;
	std::string                   publish_topic;
	std::string                   type;
	std::vector<std::pair<std::string, std::string > > rules;

	// measurements
	std::mutex                   *lock;
	uint64_t                      total;
	uint64_t                      n_samples;
} db_aggregation_t;

typedef struct
{
	// first is field from ipfix/etc to aggregate
	std::map<std::string, db_aggregation_t> aggregations;
} db_timeseries_aggregations_t;
