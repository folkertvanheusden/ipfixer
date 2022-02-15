#include <mongocxx/client.hpp>

#include "db.h"


class db_mongodb : public db
{
private:
	const std::string  database;
	const std::string  collection;
	mongocxx::client  *m_c  { nullptr };

public:
	db_mongodb(const std::string & uri, const std::string & database, const std::string & collection);
	virtual ~db_mongodb();

	bool insert(const db_record_t & dr) override;
};
