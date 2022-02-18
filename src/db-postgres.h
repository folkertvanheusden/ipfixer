#include "config.h"
#if POSTGRES_FOUND == 1
#include <pqxx/pqxx>

#include "db.h"


class db_postgres : public db
{
private:
	const std::string  host;
	const std::string  user;
	const std::string  password;
	const std::string  database;
	pqxx::connection  *connection { nullptr };

protected:
	std::string data_type_to_db_type(const data_type_t dt) override;

	std::string escape_string(const std::string & in) override;
	bool        execute_query(const std::string & q) override;
	bool        commit() override;

public:
	db_postgres(const std::string & connection_info, const db_field_mappings_t & field_mappings);
	virtual ~db_postgres();
};
#endif
