#pragma once
#include <map>
#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

#include "buffer.h"
#include "db.h"
#include "ipfix-common.h"


class ipfix
{
protected:
	std::map<uint16_t, std::vector<information_element_t> > templates;

public:
	ipfix();
	virtual ~ipfix();

	virtual bool process_packet(const uint8_t *const packet, const int packet_size, db *const target);

	static std::optional<std::string> data_to_str(const data_type_t & type, const int len, buffer & data_source);
};
