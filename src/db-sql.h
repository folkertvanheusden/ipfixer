#pragma once
#include <optional>
#include <string>

#include "db.h"
#include "db-common.h"


class db_sql : public db
{
protected:
	const db_field_mappings_t  field_mappings;

	std::string                timestamp_type { "" };
	std::string                json_type      { "" };

	virtual std::string        data_type_to_db_type(const data_type_t dt) = 0;

	virtual std::string        escape_string(const std::string & in) = 0;
	virtual bool               execute_query(const std::string & q) = 0;
	virtual bool               commit() = 0;

public:
	db_sql(const db_field_mappings_t & field_mappings);
	virtual ~db_sql();

	virtual void init_database() override;

	virtual bool insert(const db_record_t & dr);
};
