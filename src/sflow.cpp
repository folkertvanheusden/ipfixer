#include <optional>
#include <string>
#include <arpa/inet.h>

#include "logging.h"
#include "sflow.h"
#include "str.h"
// Only for enums: they're specifying SFLAddress_value as a
// union, yet its size depends on the type. Their own code
// also "manually" retrieves values from the datagrams.
#include "inmon/sflow.h"


std::optional<std::string> get_SFLAddress(buffer & b)
{
       uint32_t address_type = b.get_net_long();

       if (address_type == SFLADDRESSTYPE_IP_V4) {
               uint32_t addr = b.get_net_long();

               return myformat("%d.%d.%d.%d", (addr >> 24) & 255, (addr >> 16) & 255, (addr >> 8) & 255, addr & 255);
       }

       dolog(ll_warning, "SFLAddress_to_string: address type %u is not supported", address_type);

       return { };
}

sflow::sflow()
{
}

sflow::~sflow()
{
}

void sflow::add_to_db_record_uint32_t(db_record_t *const record, buffer & b, const std::string & name)
{
	db_record_data_t data_record { b.get_segment(4), dt_unsigned32, 4 };

	record->data.insert({ name, data_record });
}

void sflow::add_to_db_record_uint64_t(db_record_t *const record, buffer & b, const std::string & name)
{
	db_record_data_t data_record { b.get_segment(8), dt_unsigned32, 8 };

	record->data.insert({ name, data_record });
}

bool sflow::process_counters_sample_generic(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;
	
	add_to_db_record_uint32_t(&db_record, b, "ifIndex");
	add_to_db_record_uint32_t(&db_record, b, "networkType");
	add_to_db_record_uint64_t(&db_record, b, "ifSpeed");
	add_to_db_record_uint32_t(&db_record, b, "ifDirection");
	add_to_db_record_uint32_t(&db_record, b, "ifStatus");
	add_to_db_record_uint64_t(&db_record, b, "ifInOctets");
	add_to_db_record_uint32_t(&db_record, b, "ifInUcastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifInMulticastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifInBroadcastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifInDiscards");
	add_to_db_record_uint32_t(&db_record, b, "ifInErrors");
	add_to_db_record_uint32_t(&db_record, b, "ifInUnknownProtos");
	add_to_db_record_uint64_t(&db_record, b, "ifOutOctets");
	add_to_db_record_uint32_t(&db_record, b, "ifOutUcastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifOutMulticastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifOutBroadcastPkts");
	add_to_db_record_uint32_t(&db_record, b, "ifOutDiscards");
	add_to_db_record_uint32_t(&db_record, b, "ifOutErrors");
	add_to_db_record_uint32_t(&db_record, b, "ifPromiscuousMode");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_generic: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_generic: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_adaptors(const uint32_t sequence_number, buffer & b, db *const target)
{
	uint32_t num_adaptors = b.get_net_long();

	dolog(ll_debug, "sflow::process_counters_sample_adaptors: number of adaptors: %u", num_adaptors);

	for(uint32_t i=0; i<num_adaptors; i++) {
		uint32_t ifIndex  = b.get_net_long();

		uint32_t num_macs = b.get_net_long();

		for(uint32_t m=0; m<num_macs; m++) {
			buffer mac = b.get_segment(8);  // 8!

			dolog(ll_debug, "sflow::process_counters_sample_adaptors: interface %u (%u/%u), mac (%u/%u): %02x:%02x:%02x:%02x:%02x:%02x", ifIndex, i + 1, num_adaptors, m + 1, num_macs, mac.get_byte(), mac.get_byte(), mac.get_byte(), mac.get_byte(), mac.get_byte(), mac.get_byte());

			// TODO
		}
	}

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_adaptors: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	return true;
}

bool sflow::process_counters_sample_udp(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint32_t(&db_record, b, "udpInDatagrams");
	add_to_db_record_uint32_t(&db_record, b, "udpNoPorts");
	add_to_db_record_uint32_t(&db_record, b, "udpInErrors");
	add_to_db_record_uint32_t(&db_record, b, "udpOutDatagrams");
	add_to_db_record_uint32_t(&db_record, b, "udpRcvbufErrors");
	add_to_db_record_uint32_t(&db_record, b, "udpSndbufErrors");
	add_to_db_record_uint32_t(&db_record, b, "udpInCsumErrors");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_udp: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_udp: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_tcp(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint32_t(&db_record, b, "tcpRtoAlgorithm");
	add_to_db_record_uint32_t(&db_record, b, "tcpRtoMin");
	add_to_db_record_uint32_t(&db_record, b, "tcpRtoMax");
	add_to_db_record_uint32_t(&db_record, b, "tcpMaxConn");
	add_to_db_record_uint32_t(&db_record, b, "tcpActiveOpens");
	add_to_db_record_uint32_t(&db_record, b, "tcpPassiveOpens");
	add_to_db_record_uint32_t(&db_record, b, "tcpAttemptFails");
	add_to_db_record_uint32_t(&db_record, b, "tcpEstabResets");
	add_to_db_record_uint32_t(&db_record, b, "tcpCurrEstab");
	add_to_db_record_uint32_t(&db_record, b, "tcpInSegs");
	add_to_db_record_uint32_t(&db_record, b, "tcpOutSegs");
	add_to_db_record_uint32_t(&db_record, b, "tcpRetransSegs");
	add_to_db_record_uint32_t(&db_record, b, "tcpInErrs");
	add_to_db_record_uint32_t(&db_record, b, "tcpOutRsts");
	add_to_db_record_uint32_t(&db_record, b, "tcpInCsumErrors");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_tcp: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_tcp: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_icmp(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint32_t(&db_record, b, "icmpInMsgs");
	add_to_db_record_uint32_t(&db_record, b, "icmpInErrors");
	add_to_db_record_uint32_t(&db_record, b, "icmpInDestUnreachs");
	add_to_db_record_uint32_t(&db_record, b, "icmpInTimeExcds");
	add_to_db_record_uint32_t(&db_record, b, "icmpInParamProbs");
	add_to_db_record_uint32_t(&db_record, b, "icmpInSrcQuenchs");
	add_to_db_record_uint32_t(&db_record, b, "icmpInRedirects");
	add_to_db_record_uint32_t(&db_record, b, "icmpInEchos");
	add_to_db_record_uint32_t(&db_record, b, "icmpInEchoReps");
	add_to_db_record_uint32_t(&db_record, b, "icmpInTimestamps");
	add_to_db_record_uint32_t(&db_record, b, "icmpInAddrMasks");
	add_to_db_record_uint32_t(&db_record, b, "icmpInAddrMaskReps");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutMsgs");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutErrors");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutDestUnreachs");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutTimeExcds");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutParamProbs");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutSrcQuenchs");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutRedirects");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutEchos");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutEchoReps");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutTimestamps");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutTimestampReps");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutAddrMasks");
	add_to_db_record_uint32_t(&db_record, b, "icmpOutAddrMaskReps");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_icmp: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_icmp: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_ip(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint32_t(&db_record, b, "ipForwarding");
	add_to_db_record_uint32_t(&db_record, b, "ipDefaultTTL");
	add_to_db_record_uint32_t(&db_record, b, "ipInReceives");
	add_to_db_record_uint32_t(&db_record, b, "ipInHdrErrors");
	add_to_db_record_uint32_t(&db_record, b, "ipInAddrErrors");
	add_to_db_record_uint32_t(&db_record, b, "ipForwDatagrams");
	add_to_db_record_uint32_t(&db_record, b, "ipInUnknownProtos");
	add_to_db_record_uint32_t(&db_record, b, "ipInDiscards");
	add_to_db_record_uint32_t(&db_record, b, "ipInDelivers");
	add_to_db_record_uint32_t(&db_record, b, "ipOutRequests");
	add_to_db_record_uint32_t(&db_record, b, "ipOutDiscards");
	add_to_db_record_uint32_t(&db_record, b, "ipOutNoRoutes");
	add_to_db_record_uint32_t(&db_record, b, "ipReasmTimeout");
	add_to_db_record_uint32_t(&db_record, b, "ipReasmReqds");
	add_to_db_record_uint32_t(&db_record, b, "ipReasmOKs");
	add_to_db_record_uint32_t(&db_record, b, "ipReasmFails");
	add_to_db_record_uint32_t(&db_record, b, "ipFragOKs");
	add_to_db_record_uint32_t(&db_record, b, "ipFragFails");
	add_to_db_record_uint32_t(&db_record, b, "ipFragCreates");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_ip: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_ip: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_host_dsk(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint64_t(&db_record, b, "disk_total");
	add_to_db_record_uint64_t(&db_record, b, "disk_free");
	add_to_db_record_uint32_t(&db_record, b, "disk_partition_max_used");  // 100th of a percentage
	add_to_db_record_uint32_t(&db_record, b, "disk_reads");
	add_to_db_record_uint64_t(&db_record, b, "disk_bytes_read");
	add_to_db_record_uint32_t(&db_record, b, "disk_read_time");
	add_to_db_record_uint32_t(&db_record, b, "disk_writes");
	add_to_db_record_uint64_t(&db_record, b, "disk_bytes_written");
	add_to_db_record_uint32_t(&db_record, b, "disk_write_time");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_host_dsk: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_host_dsk: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample_host_mem(const uint32_t sequence_number, buffer & b, db *const target)
{
	db_record_t db_record;
	db_record.export_time           = time(nullptr);  // not in sflow data
	db_record.sequence_number       = sequence_number;
	db_record.observation_domain_id = 0;

	add_to_db_record_uint64_t(&db_record, b, "mem_total");
	add_to_db_record_uint64_t(&db_record, b, "mem_free");
	add_to_db_record_uint64_t(&db_record, b, "mem_shared");
	add_to_db_record_uint64_t(&db_record, b, "mem_buffers");
	add_to_db_record_uint64_t(&db_record, b, "mem_cached");
	add_to_db_record_uint64_t(&db_record, b, "swap_total");
	add_to_db_record_uint64_t(&db_record, b, "swap_free");
	add_to_db_record_uint32_t(&db_record, b, "page_in");
	add_to_db_record_uint32_t(&db_record, b, "page_out");
	add_to_db_record_uint32_t(&db_record, b, "swap_in");
	add_to_db_record_uint32_t(&db_record, b, "swap_out");

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample_host_mem: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	if (target->insert(db_record) == false) {
                dolog(ll_warning, "sflow::process_counters_sample_host_mem: failed inserting record into database");

		return false;
	}

	return true;
}

bool sflow::process_counters_sample(buffer & b, const bool is_expanded, db *const target)
{
	uint32_t sequence_number    = b.get_net_long();

	dolog(ll_debug, "sflow::process_counters_sample: sequence number   : %u", sequence_number);

	uint32_t ds_class           = 0;
	uint32_t ds_index           = 0;

	if (is_expanded) {
		ds_class            = b.get_net_long();
		ds_index            = b.get_net_long();
	}
	else {
		uint32_t sampler_id = b.get_net_long();
		ds_class            = sampler_id >> 24;
		ds_index            = sampler_id & 0x00ffffff;
	}

	dolog(ll_debug, "sflow::process_counters_sample: ds class          : %u", ds_class);
	dolog(ll_debug, "sflow::process_counters_sample: ds index          : %u", ds_index);

	uint32_t n_elements         = b.get_net_long();

	dolog(ll_debug, "sflow::process_counters_sample: number of elements: %u", n_elements);

	for(uint32_t i=0; i<n_elements; i++) {
		uint32_t record_type   = b.get_net_long();
		uint32_t record_length = b.get_net_long();

		dolog(ll_debug, "sflow::process_counters_sample: record %u/%u type: %u", i + 1, n_elements, record_type);
		dolog(ll_debug, "sflow::process_counters_sample: record length: %u", record_length);

		buffer record = b.get_segment(record_length);

		if (record_type == SFLCOUNTERS_GENERIC) {
			if (process_counters_sample_generic(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process generic counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_ADAPTORS) {
			if (process_counters_sample_adaptors(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process adaptors counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_UDP) {
			if (process_counters_sample_udp(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process UDP counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_TCP) {
			if (process_counters_sample_tcp(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process TCP counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_ICMP) {
			if (process_counters_sample_icmp(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process ICMP counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_IP) {
			if (process_counters_sample_ip(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process IP counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_DSK) {
			if (process_counters_sample_host_dsk(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process host-disk counters record");

				return false;
			}
		}
		else if (record_type == SFLCOUNTERS_HOST_MEM) {
			if (process_counters_sample_host_mem(sequence_number, record, target) == false) {
				dolog(ll_warning, "sflow::process_counters_sample: failed to process host-mem counters record");

				return false;
			}
		}
		else {
			dolog(ll_warning, "sflow::process_counters_sample: type of counters record %u not supported", record_type);

			return false;
		}
	}

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_counters_sample: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	return true;
}

bool sflow::process_packet(const uint8_t *const packet, const int packet_size, db *const target)
{
	buffer b(packet, packet_size);

	dolog(ll_debug, "sflow::process_packet: packet size      : %d", packet_size);

	if (packet_size < SFL_MIN_DATAGRAM_SIZE) {
		dolog(ll_warning, "sflow::process_packet: packet too small (%d bytes, expecting %zu) - not an SFLOW packet?", packet_size, SFL_MIN_DATAGRAM_SIZE);

		return false;
	}

	if (packet_size > SFL_MAX_DATAGRAM_SIZE) {
		dolog(ll_warning, "sflow::process_packet: header too small (%d bytes, expecting %zu) - not an SFLOW packet?", packet_size, SFL_MAX_DATAGRAM_SIZE);

		return false;
	}

	uint32_t packet_version = b.get_net_long();

	if (packet_version != SFLDATAGRAM_VERSION5) {
		dolog(ll_warning, "sflow::process_packet: expecting an SFLOW version %u packet, got %u", SFLDATAGRAM_VERSION5, packet_version);

		return false;
	}

	dolog(ll_debug, "sflow::process_packet: datagram version : %u", packet_version);

	auto agent_address = get_SFLAddress(b);
	if (agent_address.has_value() == false) {
		dolog(ll_warning, "sflow::process_packet: cannot determine agent address");

		return false;
	}

	dolog(ll_debug, "sflow::process_packet: agent address    : %s", agent_address.value().c_str());
	if (packet_version == SFLDATAGRAM_VERSION5)
		dolog(ll_debug, "sflow::process_packet: sub agent id     : %u", b.get_net_long());
	dolog(ll_debug, "sflow::process_packet: sequence number  : %u", b.get_net_long());
	dolog(ll_debug, "sflow::process_packet: uptime           : %u", b.get_net_long());
	uint32_t number_of_records = b.get_net_long();
	dolog(ll_debug, "sflow::process_packet: number of records: %u", number_of_records);

	for(uint32_t i=0; i<number_of_records; i++) {
		uint32_t record_type   = b.get_net_long();
		uint32_t record_length = b.get_net_long();

		dolog(ll_debug, "sflow::process_packet: record %u/%u type: %u", i + 1, number_of_records, record_type);
		dolog(ll_debug, "sflow::process_packet: record length : %u", record_length);

		buffer record = b.get_segment(record_length);

		if (record_type == SFLCOUNTERS_SAMPLE ||
		    record_type == SFLCOUNTERS_SAMPLE_EXPANDED) {
			if (process_counters_sample(record, record_type == SFLCOUNTERS_SAMPLE_EXPANDED, target) == false) {
				dolog(ll_warning, "sflow::process_packet: problem processing %scounters-sample", record_type == SFLCOUNTERS_SAMPLE_EXPANDED ? "expanded " : "");

				return false;
			}
		}
		else {
			dolog(ll_debug, "sflow::process_packet: record type %u not supported", record_type);
		}
	}

        if (b.end_reached() == false) {
                dolog(ll_warning, "sflow::process_packet: data (packet) underflow (%d bytes left)", b.get_n_bytes_left());

                return false;
        }

	return true;
}
