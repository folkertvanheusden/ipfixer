#include "config.h"
#if MARIADB_FOUND == 1
#include <mariadb/mysql.h>

#include "db.h"


class db_mysql : public db
{
private:
	const std::string         host;
	const std::string         user;
	const std::string         password;
	const std::string         database;
	MYSQL                    *handle;

protected:
	std::string data_type_to_db_type(const data_type_t dt) override;

	std::string escape_string(const std::string & in) override;
	bool        execute_query(const std::string & q) override;
	bool        commit() override;

public:
	db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database, const db_field_mappings_t & field_mappings);
	virtual ~db_mysql();
};
#endif
