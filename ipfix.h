#include <map>
#include <stdint.h>
#include <vector>


typedef struct
{
	bool     enterprise;
	uint16_t information_element_identifier;
	uint16_t field_length;
	uint32_t enterprise_number;
} information_element_t;

class ipfix
{
private:
	std::map<uint16_t, std::vector<information_element_t> > templates;

public:
	ipfix();
	virtual ~ipfix();

	bool process_packet(const uint8_t *const packet, const int packet_size);
};
