#include <mariadb/mysql.h>

#include "db.h"


class db_mysql : public db
{
private:
	const std::string  host;
	const std::string  user;
	const std::string  password;
	const std::string  database;
	MYSQL       *const handle

public:
	db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database);
	virtual ~db_mysql();

	bool insert(const db_record_t & dr) override;
};
