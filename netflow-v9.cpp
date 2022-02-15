#include <time.h>

#include "buffer.h"
#include "netflow-v9.h"
#include "logging.h"
#include "net.h"
#include "str.h"


netflow_v9::netflow_v9()
{
}

netflow_v9::~netflow_v9()
{
}

bool netflow_v9::process_packet(const uint8_t *const packet, const int packet_size, db *const target)
{
	buffer b(packet, packet_size);

	dolog(ll_debug, "process_netflow_v9_packet: packet size          : %d", packet_size);

	// message header
	uint16_t version_number  = b.get_net_short();
	uint16_t length          = b.get_net_short();
	uint32_t sysuptime       = b.get_net_long ();
	time_t   export_time     = b.get_net_long ();
	uint32_t sequence_number = b.get_net_long ();
	uint32_t source_id       = b.get_net_long ();

	dolog(ll_debug, "process_netflow_v9_packet: version number : %d", version_number);
	dolog(ll_debug, "process_netflow_v9_packet: length         : %d", length);
	dolog(ll_debug, "process_netflow_v9_packet: sysuptime      : %d", sysuptime);
	dolog(ll_debug, "process_netflow_v9_packet: export time    : %ld", export_time);
	dolog(ll_debug, "process_netflow_v9_packet: sequence number: %d", sequence_number);
	dolog(ll_debug, "process_netflow_v9_packet: source id      : %d", source_id);

	while(b.end_reached() == false) {
		// set-header
		uint16_t set_id     = b.get_net_short();
		uint16_t set_length = b.get_net_short();

		dolog(ll_debug, "process_netflow_v9_packet: set id    : %d", set_id);
		dolog(ll_debug, "process_netflow_v9_packet: set length: %d", set_length);

		if (set_length < 4) {
			dolog(ll_debug, "process_netflow_v9_packet: set length invalid, must be at least 4");
			return false;
		}

		buffer set = b.get_segment(set_length - 4);

		if (set_id == 0) {  // template record
			uint16_t template_id = set.get_net_short();
			uint16_t field_count = set.get_net_short();

			dolog(ll_debug, "process_netflow_v9_packet: template id: %d", template_id);
			dolog(ll_debug, "process_netflow_v9_packet: field count: %d", field_count);

			std::vector<information_element_t> template_;

			for(uint16_t nr=0; nr<field_count; nr++) {
				uint16_t type          = set.get_net_short();
				uint16_t field_length  = set.get_net_short();

				information_element_t ie { 0 };
				ie.enterprise                     = false;
				ie.information_element_identifier = type;
				ie.field_length                   = field_length;
				ie.enterprise_number              = 0;

				dolog(ll_debug, "process_netflow_v9_packet: field %d is type %d, length is %d bytes", nr, type, field_length);

				template_.push_back(ie);
			}

			templates.insert_or_assign(template_id, template_);

			if (set.end_reached() == false) {
				dolog(ll_warning, "process_netflow_v9_packet: data (set) underflow (%d bytes left)", set.get_n_bytes_left());

				return false;
			}
		}
		else if (set_id == 1) {  // options
			// TODO
			dolog(ll_warning, "process_netflow_v9_packet: options template sets not implemented");
		}
		else {  // data sets
			auto data_set = templates.find(set_id);

			if (data_set == templates.end()) {
				dolog(ll_debug, "process_netflow_v9_packet: template %d not set (yet)", set_id);

				return false;
			}

			dolog(ll_debug, "process_netflow_v9_packet: template %d has %zu elements", set_id, data_set->second.size());

			db_record_t db_record;
			db_record.export_time           = export_time;
			db_record.sequence_number       = sequence_number;
			db_record.observation_domain_id = source_id;

			for(auto field : data_set->second) {
				auto it = field_types.find(field.information_element_identifier);

				if (it == field_types.end()) {
					dolog(ll_warning, "process_netflow_v9_packet: information element identifier %d is not known", field.information_element_identifier);

					return false;
				}

				// value, type of value (e.g. int, float, string), length of it
				db_record_data_t drd { set.get_segment(field.field_length), it->second.second, field.field_length };

				if (log_enabled(ll_debug)) {
					buffer copy = drd.b;

					std::optional<std::string> data = data_to_str(it->second.second, field.field_length, copy);

					if (data.has_value() == false) {
						dolog(ll_debug, "process_netflow_v9_packet: information element %s of type %d: cannot convert, type not supported or invalid data", it->second.first.c_str(), it->second.second);

						return false;
					}

					dolog(ll_debug, "process_netflow_v9_packet: information element %s of type %d: \"%s\"", it->second.first.c_str(), it->second.second, data.value().c_str());
				}

				db_record.data.insert({ it->second.first, drd });
			}

			if (target)
				target->insert(db_record);
		}
	}

	if (b.end_reached() == false) {
		dolog(ll_warning, "process_netflow_v9_packet: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

		return false;
	}

	return true;
}
