#include "db-common.h"
#include "db-timeseries.h"


class db_influxdb : public db_timeseries
{
private:
	const std::string host;
	const int         port;

	std::string unescape(const db_record_t & dr, const std::string & name);

	void        aggregate(db_aggregation_t & element);

protected:
	void init_database() override;

public:
	db_influxdb(const std::string & host, const int port, db_timeseries_aggregations_t & aggregations);
	virtual ~db_influxdb();

	bool insert(const db_record_t & dr) override;
};
