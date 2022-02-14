#include <stdint.h>


class buffer
{
private:
	const uint8_t *const p { nullptr };
	const int            size { 0 };
	int                  o { 0 };

public:
	buffer(const uint8_t *p, const int size);
	virtual ~buffer();

	uint8_t  get_byte();
	uint16_t get_net_short();  // 2 bytes
	uint32_t get_net_long();  // 4 bytes
};
