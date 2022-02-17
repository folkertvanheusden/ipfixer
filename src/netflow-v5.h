#pragma once
#include <stdint.h>

#include "db.h"
#include "netflow-v9.h"


class netflow_v5 : public netflow_v9
{
public:
	netflow_v5();
	virtual ~netflow_v5();

	bool process_packet(const uint8_t *const packet, const int packet_size, db *const target);
};
