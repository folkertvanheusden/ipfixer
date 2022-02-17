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

public:
	db_postgres(const std::string & connection_info);
	virtual ~db_postgres();

	bool insert(const db_record_t & dr) override;
};
#endif
