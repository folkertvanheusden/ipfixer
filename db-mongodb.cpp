#include <bsoncxx/builder/stream/document.hpp>
#include <chrono>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "db-mongodb.h"
#include "logging.h"


mongocxx::instance instance { };

db_mongodb::db_mongodb(const std::string & uri, const std::string & database, const std::string & collection) : database(database), collection(collection)
{
	mongocxx::uri m_uri(uri);

	m_c = new mongocxx::client(m_uri);
}

db_mongodb::~db_mongodb()
{
	delete m_c;
}

bool db_mongodb::insert(const db_record_t & dr)
{
	mongocxx::database                db              = (*m_c)[database];

	mongocxx::collection              work_collection = db[collection];

	bsoncxx::builder::basic::document doc;

	doc.append(bsoncxx::builder::basic::kvp("export_time"          , bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(dr.export_time))));
	doc.append(bsoncxx::builder::basic::kvp("sequence_number"      , int64_t(dr.sequence_number)));  // 64bit because no uint32_t is understood by c++ mongo library
	doc.append(bsoncxx::builder::basic::kvp("observation_domain_id", int64_t(dr.observation_domain_id)));

	bsoncxx::builder::basic::document sub_doc;

	for(auto & key : dr.data)
		sub_doc.append(bsoncxx::builder::basic::kvp(key.first, key.second));

	doc.append(bsoncxx::builder::basic::kvp("data", sub_doc));

	bsoncxx::stdx::optional<mongocxx::result::insert_one> result = work_collection.insert_one(doc.view());

	if (!result) {
		dolog(ll_warning, "db_mongodb::insert: no result for document insert returned");

		return false;
	}

	int32_t inserted_count = result->result().inserted_count();

	if (inserted_count != 1) {
		dolog(ll_warning, "db_mongodb::insert: unexpected (%d) inserted count (expected 1)", inserted_count);

		return false;
	}

	return true;
}
