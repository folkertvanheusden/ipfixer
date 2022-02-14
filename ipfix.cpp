#include <time.h>

#include "ipfix.h"
#include "logging.h"
#include "net.h"


bool process_ipfix_packet(const uint8_t *const packet, const int packet_size)
{
	// message header
	uint16_t version_number        = get_net_short(&packet[0]);
	uint16_t length                = get_net_short(&packet[2]);
	time_t   export_time           = get_net_long (&packet[4]);
	uint32_t sequence_number       = get_net_long (&packet[8]);
	uint32_t observation_domain_id = get_net_long (&packet[12]);

	dolog(ll_debug, "process_ipfix_packet: version number       : %d", version_number);
	dolog(ll_debug, "process_ipfix_packet: length               : %d", length);
	dolog(ll_debug, "process_ipfix_packet: export time          : %ld", export_time);
	dolog(ll_debug, "process_ipfix_packet: sequence number      : %d", sequence_number);
	dolog(ll_debug, "process_ipfix_packet: observation domain id: %d", observation_domain_id);

	// set-header
	uint16_t set_id     = get_net_short(&packet[16]);
	uint16_t set_length = get_net_short(&packet[18]);

	dolog(ll_debug, "process_ipfix_packet: set id: %04x", set_id);
	dolog(ll_debug, "process_ipfix_packet: length: %d", set_length);

	int offset = 20;

	if (set_id == 2) {  // template record
		uint16_t template_id = get_net_short(&packet[offset]);
		offset += 2;
		uint16_t field_count = get_net_short(&packet[offset]);
		offset += 2;

		dolog(ll_debug, "process_ipfix_packet: template id: %04x", template_id);
		dolog(ll_debug, "process_ipfix_packet: field count: %04x", field_count);
	}
	else if (set_id >= 256) {  // data sets
	}
	else {
		dolog(ll_debug, "process_ipfix_packet: set type %d not implemented", set_id);
	}

	return true;
}
