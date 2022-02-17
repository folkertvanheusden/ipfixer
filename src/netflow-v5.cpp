#include <time.h>

#include "buffer.h"
#include "netflow-v5.h"
#include "logging.h"
#include "net.h"
#include "str.h"


netflow_v5::netflow_v5()
{
}

netflow_v5::~netflow_v5()
{
}

bool netflow_v5::process_packet(const uint8_t *const packet, const int packet_size, db *const target)
{
	buffer b(packet, packet_size);

	dolog(ll_debug, "process_netflow_v5_packet: packet size          : %d", packet_size);

	// message header
	uint16_t version_number  = b.get_net_short();

	if (version_number != 5) {
		dolog(ll_warning, "process_netflow_v5_packet: not a version 5 packet (%d instead)", version_number);

		return false;
	}

	uint16_t count             = b.get_net_short();
	uint32_t sysuptime         = b.get_net_long ();
	time_t   unix_secs         = b.get_net_long ();
	uint32_t unix_nsecs        = b.get_net_long ();
	uint32_t flow_sequence     = b.get_net_long ();
	uint8_t  engine_type       = b.get_byte     ();
	uint8_t  engine_id         = b.get_byte     ();
	uint16_t sampling_interval = b.get_net_short();

	dolog(ll_debug, "process_netflow_v5_packet: version number   : %d", version_number);
	dolog(ll_debug, "process_netflow_v5_packet: count            : %d", count);
	dolog(ll_debug, "process_netflow_v5_packet: sysuptime        : %d", sysuptime);
	dolog(ll_debug, "process_netflow_v5_packet: unix secs        : %ld", unix_secs);
	dolog(ll_debug, "process_netflow_v5_packet: unix nsecs       : %d", unix_nsecs);
	dolog(ll_debug, "process_netflow_v5_packet: flow sequence    : %d", flow_sequence);
	dolog(ll_debug, "process_netflow_v5_packet: engine type      : %d", engine_type);
	dolog(ll_debug, "process_netflow_v5_packet: engine id        : %d", engine_id);
	dolog(ll_debug, "process_netflow_v5_packet: sampling interval: %d", sampling_interval);

	while(b.end_reached() == false) {
		db_record_t dr { 0 };
		dr.export_time           = unix_secs;
		dr.sequence_number       = flow_sequence;
		dr.observation_domain_id = engine_id;

		db_record_data_t sourceIPv4Address { b.get_segment(4), dt_ipv4Address, 4 };       // source IP address
		dr.data.insert({ "sourceIPv4Address", sourceIPv4Address });

		db_record_data_t destinationIPv4Address { b.get_segment(4), dt_ipv4Address, 4 };  // destinationIPv4Address IP address
		dr.data.insert({ "destinationIPv4Address", destinationIPv4Address });

		db_record_data_t ipNextHopIPv4Address { b.get_segment(4), dt_ipv4Address, 4 };    // ipNextHopIPv4Address IP address
		dr.data.insert({ "ipNextHopIPv4Address", ipNextHopIPv4Address });

		db_record_data_t ingressInterface { b.get_segment(2), dt_unsigned32, 2 };         // SNMP index of input interface
		dr.data.insert({ "ingressInterface", ingressInterface });

		db_record_data_t egressInterface { b.get_segment(2), dt_unsigned32, 2 };          // SNMP index of output interface
		dr.data.insert({ "egressInterface", egressInterface });

		db_record_data_t packetDeltaCount { b.get_segment(4), dt_unsigned64, 4 };         // number of packets in this flow
		dr.data.insert({ "packetDeltaCount", packetDeltaCount });

		db_record_data_t octetDeltaCount { b.get_segment(4), dt_unsigned64, 4 };          // number of bytes in this flow
		dr.data.insert({ "octetDeltaCount", octetDeltaCount });

		db_record_data_t flowStartSysUpTime { b.get_segment(4), dt_unsigned32, 4 };       // system uptime of first packet in this flow
		dr.data.insert({ "flowStartSysUpTime", flowStartSysUpTime });

		db_record_data_t flowEndSysUpTime { b.get_segment(4), dt_unsigned32, 4 };        // system uptime of last packet in this flow
		dr.data.insert({ "flowEndSysUpTime", flowEndSysUpTime });

		db_record_data_t sourceTransportPort { b.get_segment(2), dt_unsigned16, 2 };      // source port
		dr.data.insert({ "sourceTransportPort", sourceTransportPort });

		db_record_data_t destinationTransportPort { b.get_segment(2), dt_unsigned16, 2 }; // destination port
		dr.data.insert({ "destinationTransportPort", destinationTransportPort });

		b.get_byte();                                                                     // pad1 (unused)

		db_record_data_t tcpControlBits { b.get_segment(1), dt_unsigned16, 1 };           // TCP flags (ORed)
		dr.data.insert({ "tcpControlBits", tcpControlBits });

		db_record_data_t protocolIdentifier { b.get_segment(1), dt_unsigned8, 1 };       // IP protocol (TCP, UDP)
		dr.data.insert({ "protocolIdentifier", protocolIdentifier });

		db_record_data_t ipClassOfService { b.get_segment(1), dt_unsigned8, 1 };         // type of service
		dr.data.insert({ "ipClassOfService", ipClassOfService });

		db_record_data_t bgpSourceAsNumber { b.get_segment(2), dt_unsigned32, 2 };       // "Autonomous system number of the source, either origin or peer"
		dr.data.insert({ "bgpSourceAsNumber", bgpSourceAsNumber });

		db_record_data_t bgpDestinationAsNumber { b.get_segment(2), dt_unsigned32, 2 };  // "Autonomous system number of the destination, either origin or peer"
		dr.data.insert({ "bgpDestinationAsNumber", bgpDestinationAsNumber });

		db_record_data_t sourceIPv4PrefixLength { b.get_segment(1), dt_unsigned8, 1 };   // source address prefix mask bits
		dr.data.insert({ "sourceIPv4PrefixLength", sourceIPv4PrefixLength });

		db_record_data_t destinationIPv4PrefixLength { b.get_segment(1), dt_unsigned8, 1 }; // destination address prefix mask bits
		dr.data.insert({ "destinationIPv4PrefixLength", destinationIPv4PrefixLength });

		b.get_net_short();                      // pad2 (unused)

		if (target)
			target->insert(dr);
	}

	if (b.end_reached() == false) {
		dolog(ll_warning, "process_netflow_v5_packet: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

		return false;
	}

	return true;
}
