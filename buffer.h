#include <stdint.h>


class buffer
{
private:
	const uint8_t *const p { nullptr };
	const int            size { 0 };
	int                  o { 0 };

	const uint8_t * get_pointer() const { return p; };
	int             get_size()    const { return size; };

public:
	buffer(const uint8_t *p, const int size);
	buffer(const buffer & b);
	virtual ~buffer();

	uint8_t  get_byte();
	uint16_t get_net_short();  // 2 bytes
	uint32_t get_net_long();  // 4 bytes

	buffer   get_segment(const int len);

	void     seek(const int len);

	bool     end_reached() const;
	int      get_n_bytes_left() const;
};
