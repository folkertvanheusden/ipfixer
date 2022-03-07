#include <functional>
#include <map>

#include "net-collect.h"
// Only for enums: they're specifying SFLAddress_value as a
// union, yet its size depends on the type. Their own code
// also "manually" retrieves values from the datagrams.
#include "inmon/sflow.h"


class sflow : public net_collect
{
private:
	std::map<SFLCounters_type_tag, std::pair<std::function<bool(const uint32_t, buffer & b, db *const)>, std::string> > sflcounters_jump_table;

protected:
	void add_to_db_record_uint32_t(db_record_t *const record, buffer & b, const std::string & name);
	void add_to_db_record_uint64_t(db_record_t *const record, buffer & b, const std::string & name);
	void add_to_db_record_float_t (db_record_t *const record, buffer & b, const std::string & name);
	void add_to_db_record_mac_t   (db_record_t *const record, buffer & b, const std::string & name);

	bool process_counters_sample_generic (const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_adaptors(const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_udp     (const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_tcp     (const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_icmp    (const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_ip      (const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_host_dsk(const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_host_mem(const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_host_cpu(const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_host_nio(const uint32_t sequence_number, buffer & b, db *const target);
	bool process_counters_sample_host_hid(const uint32_t sequence_number, buffer & b, db *const target);

	bool process_counters_sample(buffer & record, const bool is_expanded, db *const target);

public:
	sflow();
	virtual ~sflow();

	bool process_packet(const uint8_t *const packet, const int packet_size, db *const target) override;
};
