#include <map>
#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

#include "buffer.h"


typedef struct
{
	bool     enterprise;
	uint16_t information_element_identifier;
	uint16_t field_length;
	uint32_t enterprise_number;
} information_element_t;

typedef enum {
	dt_reserved = -1,
	dt_octetArray = 0,
	dt_unsigned8,
	dt_unsigned16,
	dt_unsigned32,
	dt_unsigned64,
	dt_signed8,
	dt_signed16,
	dt_signed32,
	dt_signed64,
	dt_float32,
	dt_float64,
	dt_boolean,
	dt_macAddress,
	dt_string,
	dt_dateTimeSeconds,
	dt_dateTimeMilliseconds,
	dt_dateTimeMicroseconds,
	dt_dateTimeNanoseconds,
	dt_ipv4Address,
	dt_ipv6Address,
	dt_basicList,
	dt_subTemplateList,
	dt_subTemplateMultiList
} data_type_t;

class ipfix
{
private:
	std::map<uint16_t, std::vector<information_element_t> > templates;

	std::map<uint16_t, std::pair<std::string, data_type_t> > field_types;

public:
	ipfix();
	virtual ~ipfix();

	bool process_packet(const uint8_t *const packet, const int packet_size);

	std::optional<std::string> data_to_str(const data_type_t & type, const int len, buffer & data_source);
};
