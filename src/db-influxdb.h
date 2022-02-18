#include "db-common.h"
#include "db-timeseries.h"


class db_influxdb : public db_timeseries
{
private:
	const std::string host;
	const int         port;

	std::string unescape(const db_record_t & dr, const std::string & name);

protected:
	void init_database() override;

public:
	db_influxdb(const std::string & host, const int port, const db_field_mappings_t & field_mappings);
	virtual ~db_influxdb();

	bool insert(const db_record_t & dr) override;
};
