#pragma once
#include <stdint.h>

#include "db.h"
#include "ipfix.h"


class netflow_v9 : public ipfix
{
public:
	netflow_v9();
	virtual ~netflow_v9();

	bool process_packet(const uint8_t *const packet, const int packet_size, db *const target);
};
