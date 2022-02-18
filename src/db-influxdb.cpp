#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "db-influxdb.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "str.h"


db_influxdb::db_influxdb(const std::string & host, const int port, db_timeseries_aggregations_t & aggregations) :
	db_timeseries(aggregations),
	host(host), port(port)
{
	for(auto & element : this->aggregations.aggregations) {
		element.second.lock      = new std::mutex();
		element.second.total     = 0;
		element.second.n_samples = 0;

		element.second.th        = new std::thread([this, &element] { this->aggregate(element.second); });
	}
}

db_influxdb::~db_influxdb()
{
}

void db_influxdb::init_database()
{
}

void db_influxdb::aggregate(db_aggregation_t & element)
{
        while(time(nullptr) % element.emit_interval)
		sleep(1);

	dolog(ll_info, "db_influxdb::aggregate: emit starting for %s", element.publish_topic.c_str());

	for(;;) {
		time_t now = time(nullptr);

		std::unique_lock<std::mutex> lck(*element.lock);

		if (element.n_samples) {
			std::string output;

			if (element.type == "average") {
				uint64_t avg = element.total / element.n_samples;

				output = myformat("%s %ld %ld\n", element.publish_topic.c_str(), avg, now);
			}
			else if (element.type == "sum") { 
				output = myformat("%s %ld %ld\n", element.publish_topic.c_str(), element.total, now);
			}
			else if (element.type == "count") { 
				output = myformat("%s %ld %ld\n", element.publish_topic.c_str(), element.n_samples, now);
			}

			element.n_samples = element.total = 0;

			lck.unlock();

			int fd = connect_to(host, port);
			if (fd == -1)
				dolog(ll_info, "db_influxdb::insert: cannot connect to [%s]:%d", host.c_str(), port);
			else {
				if (WRITE(fd, reinterpret_cast<const uint8_t *>(output.c_str()), output.size()) != ssize_t(output.size()))
					dolog(ll_info, "db_influxdb::insert: cannot transmit to [%s]:%d", host.c_str(), port);

				close(fd);
			}
		}
		else {
			lck.unlock();
		}

		if (now == time(nullptr))
			sleep(1);

	        while(time(nullptr) % element.emit_interval)
			sleep(1);
	}
}

bool db_influxdb::insert(const db_record_t & dr)
{
	db_record_t work = dr;

	for(auto & element : work.data) {
		// find aggregation (if any) by name of the flow-element
		auto it = aggregations.aggregations.find(element.first);
		if (it == aggregations.aggregations.end())
			continue;

		std::unique_lock lck(*it->second.lock);

		// check the rules if any
		bool use = true;

		for(auto & rule : it->second.rules) {
			// get field from work.data
			auto field_it = work.data.find(rule.first);

			// field not in set? then this rule doesn't
			// match, abort
			if (field_it == work.data.end()) {
				use = false;
				break;
			}

			// get the (processed) value of the field
			auto value = ipfix::data_to_str(field_it->second.dt, field_it->second.len, field_it->second.b);

			// can't process this field, abort rule(s)
			if (value.has_value() == false) {
				dolog(ll_info, "db_influxdb::insert: cannot retrieve value from field \"%s\"", rule.first.c_str());

				use = false;
				break;
			}

			// contents mismatch
			if (value.value() != rule.second) {
				use = false;
				break;
			}
		}

		if (use) {
			// update record with sample value
			if (element.second.dt == dt_unsigned8 ||
			    element.second.dt == dt_unsigned16 ||
			    element.second.dt == dt_unsigned32 ||
			    element.second.dt == dt_unsigned64 ||
			    element.second.dt == dt_signed8 ||
			    element.second.dt == dt_signed16 ||
			    element.second.dt == dt_signed32 ||
			    element.second.dt == dt_signed64) {
				auto value = ipfix::data_to_str(element.second.dt, element.second.len, element.second.b);

				if (value.has_value() == true) {
					it->second.total += std::atoll(value.value().c_str());
					it->second.n_samples++;
				}
				else {
					dolog(ll_info, "db_influxdb::insert: cannot retrieve value from field \"%s\"", element.first.c_str());
				}
			}

			// TODO process other data-types
			// e.g. update a timestamp to latest
		}
	}

	return true;
}
