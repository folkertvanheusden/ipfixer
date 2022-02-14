#include <time.h>

#include "ipfix.h"
#include "logging.h"
#include "net.h"


bool process_ipfix_packet(const uint8_t *const packet, const int packet_size)
{
	uint16_t version_number        = get_net_short(&packet[0]);
	uint16_t length                = get_net_short(&packet[2]);
	time_t   export_time           = get_net_long (&packet[4]);
	uint32_t sequence_number       = get_net_long (&packet[8]);
	uint32_t observation_domain_id = get_net_long (&packet[12]);

	dolog(ll_debug, "version number       : %d", version_number);
	dolog(ll_debug, "length               : %d", length);
	dolog(ll_debug, "export time          : %ld", export_time);
	dolog(ll_debug, "sequence number      : %d", sequence_number);
	dolog(ll_debug, "observation domain id: %d", observation_domain_id);


	return true;
}
