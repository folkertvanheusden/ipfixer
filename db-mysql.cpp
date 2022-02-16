#include <jansson.h>
#include <mariadb/mysql.h>

#include "db-mysql.h"
#include "error.h"
#include "logging.h"


void exec_query(MYSQL *const handle, const std::string & query)
{
	if (mysql_query(handle, query.c_str()))
		error_exit(false, "exec_query: query \"%s\" failed, reason: %s", query.c_str(), mysql_error(handle));
}

void start_transaction(MYSQL *const handle)
{
	exec_query(handle, "begin");
}

void commit_transaction(MYSQL *const handle)
{
	exec_query(handle, "commit");
}

db_mysql::db_mysql(const std::string & host, const std::string & user, const std::string & password, const std::string & database)
{
	handle = mysql_init(nullptr);
	if (!handle)
		error_exit(false, "db_mysql: failed to initialize MySQL library");

        if (mysql_real_connect(handle, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0) == 0)
		error_exit(false, "db_mysql: failed to connect to MySQL database, season: %s", mysql_error(handle));
}

db_mysql::~db_mysql()
{
}

bool db_mysql::insert(const db_record_t & dr)
{
	doc.append(bsoncxx::builder::basic::kvp("export_time"          , bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(dr.export_time))));
	// 64bit because no 32 bit unsigned is understood by c++ mongo library:
	doc.append(bsoncxx::builder::basic::kvp("sequence_number"      , int64_t(dr.sequence_number)));
	doc.append(bsoncxx::builder::basic::kvp("observation_domain_id", int64_t(dr.observation_domain_id)));

	bsoncxx::builder::basic::document sub_doc;

	for(auto & element : dr.data) {
		db_record_data_t element_data = element.second;

		switch(element_data.dt) {
			case dt_octetArray:
			case dt_ipv4Address:
			case dt_ipv6Address:
			case dt_macAddress: {
					const uint8_t *bytes = element_data.b.get_bytes(element_data.len);

					bsoncxx::types::b_binary binary_data;
					binary_data.size = element_data.len;
					binary_data.bytes = bytes;

					sub_doc.append(bsoncxx::builder::basic::kvp(element.first, binary_data));
				}
				break;

			case dt_unsigned8:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_byte()));
				break;

			case dt_unsigned16: {
					uint16_t temp = get_variable_size_integer(element_data.b, element_data.len);

					sub_doc.append(bsoncxx::builder::basic::kvp(element.first, temp));
				}
				break;

			case dt_unsigned32: {
					uint32_t temp = get_variable_size_integer(element_data.b, element_data.len);

					if (temp > 2147483647) 
						sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int64_t(temp)));
					else
						sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int32_t(temp)));
				}
				break;

			case dt_unsigned64: {
					uint64_t temp = get_variable_size_integer(element_data.b, element_data.len);

					// hope for the best! (will fail when value > 0x7fffffffffffffff)
					sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int64_t(temp)));
				}
				break;

			case dt_signed8:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_byte()));
				break;

			case dt_signed16:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_net_short()));
				break;

			case dt_signed32:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int32_t(element_data.b.get_net_long())));
				break;

			case dt_signed64:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int64_t(element_data.b.get_net_long_long())));
				break;

			case dt_float32:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_net_float()));
				break;

			case dt_float64:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_net_double()));
				break;

			case dt_boolean: {
					uint8_t v = element_data.b.get_byte();

					if (v == 1)
						sub_doc.append(bsoncxx::builder::basic::kvp(element.first, true));
					else if (v == 2)
						sub_doc.append(bsoncxx::builder::basic::kvp(element.first, false));
					else
						dolog(ll_warning, "db_mysql::insert: unexpected value %d found for type boolean, expected 1 or 2", v);
				 }
				break;

			case dt_string:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, element_data.b.get_string(element_data.b.get_n_bytes_left())));
				break;

			case dt_dateTimeSeconds:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, bsoncxx::types::b_date(std::chrono::system_clock::from_time_t(element_data.b.get_net_long()))));
				break;

			case dt_dateTimeMilliseconds: {
					std::chrono::milliseconds m(element_data.b.get_net_long_long());

					sub_doc.append(bsoncxx::builder::basic::kvp(element.first, bsoncxx::types::b_date(m)));
				}
				break;

			case dt_dateTimeMicroseconds:
			case dt_dateTimeNanoseconds:
				sub_doc.append(bsoncxx::builder::basic::kvp(element.first, int64_t(element_data.b.get_net_long_long())));
				break;

//			case dt_basicList:  TODO
//			case dt_subTemplateList:  TODO
//			case dt_subTemplateMultiList:  TODO
			default:
				dolog(ll_warning, "db_mysql::insert: data type %d not supported for MongoDB target", element_data.dt);

				return false;
		}
	}

	doc.append(bsoncxx::builder::basic::kvp("data", sub_doc));

	bsoncxx::stdx::optional<mongocxx::result::insert_one> result = work_collection.insert_one(doc.view());

	if (!result) {
		dolog(ll_warning, "db_mysql::insert: no result for document insert returned");

		return false;
	}

	int32_t inserted_count = result->result().inserted_count();

	if (inserted_count != 1) {
		dolog(ll_warning, "db_mysql::insert: unexpected (%d) inserted count (expected 1)", inserted_count);

		return false;
	}

	return true;
}
