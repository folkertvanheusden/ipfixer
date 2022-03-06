#pragma once
#include <stdint.h>

#include "db.h"


class net_collect
{
public:
	net_collect();
	virtual ~net_collect();

	virtual bool process_packet(const uint8_t *const packet, const int packet_size, db *const target) = 0;
};
