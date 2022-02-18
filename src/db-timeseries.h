#include "db.h"
#include "db-common.h"


class db_timeseries : public db
{
protected:
	db_timeseries_aggregations_t aggregations;

public:
	db_timeseries(db_timeseries_aggregations_t & aggregations);
	virtual ~db_timeseries();
};
