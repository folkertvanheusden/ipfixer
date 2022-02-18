#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "db-influxdb.h"
#include "error.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "str.h"


db_influxdb::db_influxdb(const std::string & host, const int port, const db_field_mappings_t & field_mappings) :
	db_timeseries(field_mappings),
	host(host), port(port)
{
}

db_influxdb::~db_influxdb()
{
}

void db_influxdb::init_database()
{
}

std::string db_influxdb::unescape(const db_record_t & dr, const std::string & name)
{
	std::string work_str = name;

	for(;;) {
		// curly bracket start
		std::size_t cb_start = work_str.find('{');
		if (cb_start == std::string::npos)
			break;

		// curly bracket end
		std::size_t cb_end = work_str.find('}');
		if (cb_end == std::string::npos)
			error_exit(false, "db_influxdb::unescape: \"%s\" is invalid, missing '}'", name.c_str());

		std::string front      = work_str.substr(0, cb_start);
		std::string back       = work_str.substr(cb_end + 1);

		std::string temp       = work_str.substr(cb_start + 1, cb_end - cb_start - 1);
		std::size_t colon      = temp.find(':');
		std::string escape_key;
		std::string escape_val;

		if (colon != std::string::npos) {
			escape_key = temp.substr(0, colon);
			escape_val = temp.substr(colon + 1);
		}
		else {
			escape_key = temp;
		}

		std::string result;

		if (escape_key == "my-hostname") {
			struct utsname un { 0 };

			if (uname(&un) == -1)
				error_exit(true, "db_influxdb::unescape: \"utsname\" failed");

			result = un.nodename;
		}
		else if (escape_key == "value") {
			result = "_";  // default value in case of error

			auto value_it = dr.data.find(escape_val);
			if (value_it == dr.data.end())
				dolog(ll_info, "db_influxdb::unescape: \"%s\"; key not found", escape_val.c_str());
			else {
				buffer copy_b = value_it->second.b;

				auto temp = ipfix::data_to_str(value_it->second.dt, value_it->second.len, copy_b);
				if (temp.has_value() == false) 
					dolog(ll_info, "db_influxdb::unescape: problem converting \"%s\"-data", escape_val.c_str());
				else
					result = temp.value();
			}
		}
		else {
			error_exit(false, "db_influxdb::unescape: \"%s\" is an unknown escape'", escape_key.c_str());
		}

		work_str = front + result + back;
	}

	return work_str;
}

bool db_influxdb::insert(const db_record_t & dr)
{
	time_t      now  = time(nullptr);

	db_record_t work = dr;

	int fd = connect_to(host, port);
	if (fd == -1) {
		dolog(ll_info, "db_influxdb::insert: cannot connect to [%s]:%d", host.c_str(), port);

		return false;
	}

	std::string output;

	for(auto & element : work.data) {
		std::string key   = unescape(work, element.first);
		auto      & value = element.second;

		auto temp = ipfix::data_to_str(value.dt, value.len, value.b);
		if (temp.has_value() == false) {
			dolog(ll_info, "db_influxdb::insert: cannot retrieve value \"%s\"", element.first.c_str());

			return false;
		}

		output += myformat("%s %s %ld\n", key.c_str(), temp.value().c_str(), now);
	}

	if (WRITE(fd, reinterpret_cast<const uint8_t *>(output.c_str()), output.size()) != ssize_t(output.size())) {
		dolog(ll_info, "db_influxdb::insert: cannot transmit to [%s]:%d", host.c_str(), port);

		return false;
	}

	close(fd);

	return true;
}
