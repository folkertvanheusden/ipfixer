#include "config.h"
#if LIBMONGOCXX_FOUND == 1
#include <mongocxx/client.hpp>

#include "db.h"


class db_mongodb : public db
{
private:
	const db_field_mappings_t  field_mappings;

	const std::string          database;
	const std::string          collection;
	mongocxx::client          *m_c  { nullptr };

public:
	db_mongodb(const std::string & uri, const std::string & database, const std::string & collection, const db_field_mappings_t & field_mappings);
	virtual ~db_mongodb();

	void init_database() override;

	bool insert(const db_record_t & dr) override;
};
#endif
