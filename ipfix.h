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
private:
	std::map<uint16_t, std::vector<information_element_t> > templates;

	std::map<uint16_t, std::pair<std::string, data_type_t> > field_types;

public:
	ipfix();
	virtual ~ipfix();

	bool process_packet(const uint8_t *const packet, const int packet_size, db *const target);

	std::optional<std::string> data_to_str(const data_type_t & type, const int len, buffer & data_source);
};
