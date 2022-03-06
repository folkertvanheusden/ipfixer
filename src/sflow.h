#include "net-collect.h"


class sflow : public net_collect
{
protected:
	void add_to_db_record_uint32_t(db_record_t *const record, buffer & b, const std::string & name);
	void add_to_db_record_uint64_t(db_record_t *const record, buffer & b, const std::string & name);
	bool process_counters_sample_generic(buffer & b, db *const target);
	bool process_counters_sample(buffer & record, const bool is_expanded, db *const target);

public:
	sflow();
	virtual ~sflow();

	bool process_packet(const uint8_t *const packet, const int packet_size, db *const target) override;
};
