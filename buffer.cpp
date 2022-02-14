#include <stdexcept>
#include <stdint.h>

#include "buffer.h"
#include "net.h"


buffer::buffer(const uint8_t *p, const int size) : p(p), size(size)
{
}

buffer::~buffer()
{
}

uint8_t  buffer::get_byte()
{
	if (o >= size)
		throw std::out_of_range("buffer::get_byte");

	return p[o++];
}

uint16_t buffer::get_net_short()
{
	if (o + 2 > size)
		throw std::out_of_range("buffer::get_net_short");

	uint16_t temp = ::get_net_short(&p[o]);
	o += 2;

	return temp;
}

uint32_t buffer::get_net_long()
{
	if (o + 4 > size)
		throw std::out_of_range("buffer::get_net_long");

	uint16_t temp = ::get_net_long(&p[o]);
	o += 4;

	return temp;
}
