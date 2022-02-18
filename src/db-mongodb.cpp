#include "config.h"
#if LIBMONGOCXX_FOUND == 1
#include <bsoncxx/builder/stream/document.hpp>
#include <chrono>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "db-mongodb.h"
#include "logging.h"


mongocxx::instance instance { };

db_mongodb::db_mongodb(const std::string & uri, const std::string & database, const std::string & collection, const db_field_mappings_t & field_mappings) :
	db(field_mappings),
	database(database), collection(collection)
{
	mongocxx::uri m_uri(uri);

	m_c = new mongocxx::client(m_uri);
}

db_mongodb::~db_mongodb()
{
	delete m_c;
}

void db_mongodb::init_database()
{
	// no-op
}

std::string db_mongodb::data_type_to_db_type(const data_type_t dt)
{
	return nullptr;
}

std::string db_mongodb::escape_string(const std::string & in)
{
	return nullptr;
}

bool db_mongodb::execute_query(const std::string & q)
{
	return false;
}

bool db_mongodb::commit()
{
	return false;
}

bool db_mongodb::insert(const db_record_t & dr)
{
	mongocxx::database                db              = (*m_c)[database];

	mongocxx::collection              work_collection = db[collection];

	bsoncxx::builder::basic::document doc;

	doc.append(bsoncxx::builder::basic::kvp("export_time"          , bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(dr.export_time))));
	// 64bit because no 32 bit unsigned is understood by c++ mongo library:
	doc.append(bsoncxx::builder::basic::kvp("sequence_number"      , int64_t(dr.sequence_number)));
	doc.append(bsoncxx::builder::basic::kvp("observation_domain_id", int64_t(dr.observation_domain_id)));

	bsoncxx::builder::basic::document sub_doc;

	for(auto & element : dr.data) {
		db_record_data_t element_data = element.second;

		std::string key_name = element.first;

		auto it = field_mappings.mappings.find(key_name);
		if (it != field_mappings.mappings.end())
			key_name = it->second.target_name;

		switch(element_data.dt) {
			case dt_octetArray:
			case dt_ipv4Address:
			case dt_ipv6Address:
			case dt_macAddress: {
					const uint8_t *bytes = element_data.b.get_bytes(element_data.len);

					bsoncxx::types::b_binary binary_data;
					binary_data.size = element_data.len;
					binary_data.bytes = bytes;

					sub_doc.append(bsoncxx::builder::basic::kvp(key_name, binary_data));
				}
				break;

			case dt_unsigned8:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_byte()));
				break;

			case dt_unsigned16: {
					uint16_t temp = get_variable_size_integer(element_data.b, element_data.len);

					sub_doc.append(bsoncxx::builder::basic::kvp(key_name, temp));
				}
				break;

			case dt_unsigned32: {
					uint32_t temp = get_variable_size_integer(element_data.b, element_data.len);

					if (temp > 2147483647) 
						sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int64_t(temp)));
					else
						sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int32_t(temp)));
				}
				break;

			case dt_unsigned64: {
					uint64_t temp = get_variable_size_integer(element_data.b, element_data.len);

					// hope for the best! (will fail when value > 0x7fffffffffffffff)
					sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int64_t(temp)));
				}
				break;

			case dt_signed8:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_byte()));
				break;

			case dt_signed16:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_net_short()));
				break;

			case dt_signed32:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int32_t(element_data.b.get_net_long())));
				break;

			case dt_signed64:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int64_t(element_data.b.get_net_long_long())));
				break;

			case dt_float32:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_net_float()));
				break;

			case dt_float64:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_net_double()));
				break;

			case dt_boolean: {
					uint8_t v = element_data.b.get_byte();

					if (v == 1)
						sub_doc.append(bsoncxx::builder::basic::kvp(key_name, true));
					else if (v == 2)
						sub_doc.append(bsoncxx::builder::basic::kvp(key_name, false));
					else
						dolog(ll_warning, "db_mongodb::insert: unexpected value %d found for type boolean, expected 1 or 2", v);
				 }
				break;

			case dt_string:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, element_data.b.get_string(element_data.b.get_n_bytes_left())));
				break;

			case dt_dateTimeSeconds:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(element_data.b.get_net_long()))));
				break;

			case dt_dateTimeMilliseconds: {
					std::chrono::milliseconds m(element_data.b.get_net_long_long());

					sub_doc.append(bsoncxx::builder::basic::kvp(key_name, bsoncxx::types::b_date(m)));
				}
				break;

			case dt_dateTimeMicroseconds:
			case dt_dateTimeNanoseconds:
				sub_doc.append(bsoncxx::builder::basic::kvp(key_name, int64_t(element_data.b.get_net_long_long())));
				break;

//			case dt_basicList:  TODO
//			case dt_subTemplateList:  TODO
//			case dt_subTemplateMultiList:  TODO
			default:
				dolog(ll_warning, "db_mongodb::insert: data type %d not supported for MongoDB target", element_data.dt);

				return false;
		}
	}

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
#endif
