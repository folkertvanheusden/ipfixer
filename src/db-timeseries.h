#include "db.h"
#include "db-common.h"


class db_timeseries : public db
{
public:
	db_timeseries(const db_field_mappings_t & field_mappings);
	virtual ~db_timeseries();
};
