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
	const db_field_mappings_t field_mappings;
	MYSQL                    *handle;

public:
	db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database, const db_field_mappings_t & field_mappings);
	virtual ~db_mysql();

	bool insert(const db_record_t & dr) override;
};
#endif
