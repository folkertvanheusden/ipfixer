#include "config.h"
#if LIBMONGOCXX_FOUND == 1
#include <mongocxx/client.hpp>

#include "db.h"


class db_mongodb : public db
{
private:
	const std::string  database;
	const std::string  collection;
	mongocxx::client  *m_c  { nullptr };

protected:
	// not used for MongoDB
	std::string data_type_to_db_type(const data_type_t dt);
	std::string escape_string(const std::string & in);
	bool        execute_query(const std::string & q);
	bool        commit();

public:
	db_mongodb(const std::string & uri, const std::string & database, const std::string & collection, const db_field_mappings_t & field_mappings);
	virtual ~db_mongodb();

	bool insert(const db_record_t & dr) override;
};
#endif
