#include <time.h>

#include "buffer.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"
#include "str.h"


ipfix::ipfix()
{
}

ipfix::~ipfix()
{
	templates.clear();
}

bool ipfix::process_packet(const uint8_t *const packet, const int packet_size, db *const target)
{
	buffer b(packet, packet_size);

	dolog(ll_debug, "process_ipfix_packet: packet size          : %d", packet_size);

	// message header
	uint16_t version_number        = b.get_net_short();
	if (version_number != 10) {
		dolog(ll_warning, "process_netflow_v5_packet: not a IPFIX packet (NetFlow v%d instead)", version_number);

		return false;
	}

	uint16_t length                = b.get_net_short();
	time_t   export_time           = b.get_net_long ();
	uint32_t sequence_number       = b.get_net_long ();
	uint32_t observation_domain_id = b.get_net_long ();

	dolog(ll_debug, "process_ipfix_packet: version number       : %d", version_number);
	dolog(ll_debug, "process_ipfix_packet: length               : %d", length);
	dolog(ll_debug, "process_ipfix_packet: export time          : %ld", export_time);
	dolog(ll_debug, "process_ipfix_packet: sequence number      : %d", sequence_number);
	dolog(ll_debug, "process_ipfix_packet: observation domain id: %d", observation_domain_id);

	while(b.end_reached() == false) {
		// set-header
		uint16_t set_id     = b.get_net_short();
		uint16_t set_length = b.get_net_short();

		dolog(ll_debug, "process_ipfix_packet: set id    : %d", set_id);
		dolog(ll_debug, "process_ipfix_packet: set length: %d", set_length);

		if (set_length < 4) {
			dolog(ll_debug, "process_ipfix_packet: set length invalid, must be at least 4");
			return false;
		}

		buffer set = b.get_segment(set_length - 4);

		if (set_id == 0 || set_id == 1) {  // not used (RFC3954)
			dolog(ll_debug, "process_ipfix_packet: set id 0 and 1 should not occur");

			return false;
		}
		else if (set_id == 2) {  // template record
			uint16_t template_id = set.get_net_short();
			uint16_t field_count = set.get_net_short();

			dolog(ll_debug, "process_ipfix_packet: template id: %d", template_id);
			dolog(ll_debug, "process_ipfix_packet: field count: %d", field_count);

			std::vector<information_element_t> template_;

			for(uint16_t nr=0; nr<field_count; nr++) {
				uint16_t ie_identifier     = set.get_net_short();  // information element identifier
				bool     enterprise        = !!(ie_identifier & 32768);
				uint16_t field_length      = set.get_net_short();
				uint32_t enterprise_number = 0;

				ie_identifier &= 32767;

				if (enterprise)
					enterprise_number  = set.get_net_long();

				information_element_t ie { 0 };
				ie.enterprise                     = enterprise;
				ie.information_element_identifier = ie_identifier;
				ie.field_length                   = field_length;
				ie.enterprise_number              = enterprise_number;

				if (enterprise)
					dolog(ll_debug, "process_ipfix_packet: field %d is type %d and is enterprise, length is %d bytes (enterprise number: %d)", nr, ie_identifier, field_length, enterprise_number);
				else
					dolog(ll_debug, "process_ipfix_packet: field %d is type %d and is not enterprise, length is %d bytes", nr, ie_identifier, field_length);

				template_.push_back(ie);
			}

			templates.insert_or_assign(template_id, template_);

			if (set.end_reached() == false) {
				dolog(ll_warning, "process_ipfix_packet: data (set) underflow (%d bytes left)", set.get_n_bytes_left());

				return false;
			}
		}
		else if (set_id == 3) {  // options template sets
			dolog(ll_warning, "process_ipfix_packet: options template sets not implemented");

			// return false;
		}
		else if (set_id >= 256) {  // data sets
			auto data_set = templates.find(set_id);

			if (data_set == templates.end()) {
				dolog(ll_debug, "process_ipfix_packet: template %d not set (yet)", set_id);

				return false;
			}

			dolog(ll_debug, "process_ipfix_packet: template %d has %zu elements", set_id, data_set->second.size());

			db_record_t db_record;
			db_record.export_time           = export_time;
			db_record.sequence_number       = sequence_number;
			db_record.observation_domain_id = observation_domain_id;

			for(auto field : data_set->second) {
				auto element = field_lookup.get_data(field.information_element_identifier);

				if (element.has_value() == false) {
					dolog(ll_warning, "process_ipfix_packet: information element identifier %d is not known", field.information_element_identifier);

					return false;
				}

				// value, type of value (e.g. int, float, string), length of it
				db_record_data_t drd { set.get_segment(field.field_length), element.value().second, field.field_length };

				if (log_enabled(ll_debug)) {
					buffer copy = drd.b;

					std::optional<std::string> data = data_to_str(element.value().second, field.field_length, copy);

					if (data.has_value() == false) {
						dolog(ll_debug, "process_ipfix_packet: information element %s of type %d: cannot convert, type not supported or invalid data", element.value().first.c_str(), element.value().second);

						return false;
					}

					dolog(ll_debug, "process_ipfix_packet: information element %s of type %d: \"%s\"", element.value().first.c_str(), element.value().second, data.value().c_str());
				}

				db_record.data.insert({ element.value().first, drd });
			}

			if (target)
				target->insert(db_record);
		}
		else {
			dolog(ll_error, "process_ipfix_packet: set type %d not implemented", set_id);
		}
	}

	if (b.end_reached() == false) {
		dolog(ll_warning, "process_ipfix_packet: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

		return false;
	}

	return true;
}

std::optional<std::string> ipfix::data_to_str(const data_type_t & type, const int len, buffer & data_source)
{
	std::optional<std::string> out = { };
	int expected_length = -1;

	switch(type) {
		case dt_octetArray: {
				expected_length = len;

				std::string temp;

				for(int i=0; i<len; i++)
					temp += myformat("%02x", data_source.get_byte());

				out = temp;
		    	}
			break;

		case dt_unsigned8:
			expected_length = 1;
			out = myformat("%u", data_source.get_byte());
			break;

		case dt_unsigned16:
			expected_length = len <= 2 ? len : 2;
			out = myformat("%u", uint16_t(get_variable_size_integer(data_source, len)));
			break;

		case dt_unsigned32:
			expected_length = len <= 4 ? len : 4;
			out = myformat("%u", uint32_t(get_variable_size_integer(data_source, len)));
			break;

		case dt_unsigned64:
			expected_length = len <= 8 ? len : 8;
			out = myformat("%lu", get_variable_size_integer(data_source, len));
			break;

		case dt_signed8:
			expected_length = 1;
			out = myformat("%d", static_cast<int8_t>(data_source.get_byte()));
			break;

		case dt_signed16:
			expected_length = 2;
			out = myformat("%d", static_cast<int16_t>(data_source.get_net_short()));
			break;

		case dt_signed32:
			expected_length = 4;
			out = myformat("%d", static_cast<int32_t>(data_source.get_net_long()));
			break;

		case dt_signed64:
			expected_length = 8;
			out = myformat("%ld", static_cast<int64_t>(data_source.get_net_long_long()));
			break;

		case dt_float32: {
				expected_length = 4;
				uint32_t temp = data_source.get_net_long();
				out = myformat("%f", *reinterpret_cast<float *>(&temp));
			}
			break;

		case dt_float64: {
				expected_length = 8;
				uint64_t temp = data_source.get_net_long();
				out = myformat("%f", *reinterpret_cast<double *>(&temp));
			}
			break;

		case dt_boolean: {
				expected_length = 8;
				uint8_t v = data_source.get_byte();

				if (v == 1)
					out = "true";
				else if (v == 2)
					out = "false";
				else
					dolog(ll_warning, "ipfix::data_to_str: unexpected value %d found for type %d, expected 1 or 2", v, type);
			}
			break;

		case dt_macAddress: {
				expected_length = 6;

				uint8_t bytes[6] { 0 };
				for(int i=0; i<6; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%02x:%02x:%02x:%02x:%02x:%02x", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
			}
			break;

		case dt_string:
			expected_length = len;
			out = data_source.get_string(len);
			break;

		case dt_dateTimeSeconds:
			expected_length = 4;
			out = myformat("%u", data_source.get_net_long());
			break;

		case dt_dateTimeMilliseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_dateTimeMicroseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_dateTimeNanoseconds:
			expected_length = 8;
			out = myformat("%lu", data_source.get_net_long_long());
			break;

		case dt_ipv4Address: {
				expected_length = 4;

				uint8_t bytes[4] { 0 };
				for(int i=0; i<4; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
			}
			break;

		case dt_ipv6Address: {
				expected_length = 16;

				uint8_t bytes[16] { 0 };
				for(int i=0; i<16; i++)
					bytes[i] = data_source.get_byte();

				out = myformat("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
						bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7],
						bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
			}
			break;

		// case dt_basicList,
		// case dt_subTemplateList,
		// case dt_subTemplateMultiList

		default:
			data_source.seek(len);
			break;
	}

	if (expected_length != len) {
		dolog(ll_warning, "ipfix::data_to_str: unexpected length %d found for type %d, expected %d", len, type, expected_length);
		return { };
	}

	return out;
}
