#include <time.h>

#include "buffer.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"


ipfix::ipfix()
{
}

ipfix::~ipfix()
{
}

bool ipfix::process_packet(const uint8_t *const packet, const int packet_size)
{
	buffer b(packet, packet_size);

	// message header
	uint16_t version_number        = b.get_net_short();
	uint16_t length                = b.get_net_short();
	time_t   export_time           = b.get_net_long ();
	uint32_t sequence_number       = b.get_net_long ();
	uint32_t observation_domain_id = b.get_net_long ();

	dolog(ll_debug, "process_ipfix_packet: version number       : %d", version_number);
	dolog(ll_debug, "process_ipfix_packet: length               : %d", length);
	dolog(ll_debug, "process_ipfix_packet: export time          : %ld", export_time);
	dolog(ll_debug, "process_ipfix_packet: sequence number      : %d", sequence_number);
	dolog(ll_debug, "process_ipfix_packet: observation domain id: %d", observation_domain_id);

	// set-header
	uint16_t set_id     = b.get_net_short();
	uint16_t set_length = b.get_net_short();

	buffer   set        = b.get_segment(set_length);

	dolog(ll_debug, "process_ipfix_packet: set id: %04x", set_id);
	dolog(ll_debug, "process_ipfix_packet: length: %d", set_length);

	if (set_id == 2) {  // template record
		uint16_t template_id = set.get_net_short();
		uint16_t field_count = set.get_net_short();

		dolog(ll_debug, "process_ipfix_packet: template id: %04x", template_id);
		dolog(ll_debug, "process_ipfix_packet: field count: %04x", field_count);

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

			dolog(ll_debug, "process_ipfix_packet: field %d is type %d and is %senterprise, length is %d bytes (enterprise number: %d)", nr, ie_identifier, enterprise ? "" : "not ", field_length, enterprise_number);

			template_.push_back(ie);
		}

		templates.insert_or_assign(template_id, template_);
	}
	else if (set_id >= 256) {  // data sets
	}
	else {
		dolog(ll_debug, "process_ipfix_packet: set type %d not implemented", set_id);
	}

	return true;
}
