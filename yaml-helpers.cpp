#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "str.h"


std::string yaml_get_string(const YAML::Node & node, const std::string & key, const std::string & description)
{
	try {
		return node[key].as<std::string>();
	}
	catch(YAML::InvalidNode & yin) {
		throw myformat("yaml_get_string: item \"%s\" (%s) is missing in YAML file", key.c_str(), description.c_str());
	}
}

int yaml_get_int(const YAML::Node & node, const std::string & key, const std::string & description)
{
	try {
		return node[key].as<int>();
	}
	catch(YAML::InvalidNode & yin) {
		throw myformat("yaml_get_int: item \"%s\" (%s) is missing in YAML file", key.c_str(), description.c_str());
	}
}

uint64_t yaml_get_uint64_t(const YAML::Node & node, const std::string & key, const std::string & description, const bool units)
{
	try {
		if (units) {
			std::string value = str_tolower(node[key].as<std::string>());

			char unit = value.at(value.size() - 1);
			uint64_t mul = 1;

			if (unit == 'g')
				mul = 1024 * 1024 * 1024;
			else if (unit == 'm')
				mul = 1024 * 1024;
			else if (unit == 'k')
				mul = 1024;
			else if (unit >= 'a' && unit <= 'z')
				throw myformat("yaml_get_int: unit \"%c\" is not known/supported", unit);

			return uint64_t(atoll(value.c_str())) * mul;
		}

		return node[key].as<uint64_t>();
	}
	catch(YAML::InvalidNode & yin) {
		throw myformat("yaml_get_uint64_t: item \"%s\" (%s) is missing in YAML file", key.c_str(), description.c_str());
	}
}

const YAML::Node yaml_get_yaml_node(const YAML::Node & node, const std::string & key, const std::string & description)
{
	try {
		return node[key];
	}
	catch(YAML::InvalidNode & yin) {
		throw myformat("yaml_get_yaml_node: item \"%s\" (%s) is missing in YAML file", key.c_str(), description.c_str());
	}
}

bool yaml_get_bool(const YAML::Node & node, const std::string & key, const std::string & description)
{
	try {
		return node[key].as<bool>();
	}
	catch(YAML::InvalidNode & yin) {
		throw myformat("yaml_get_bool: item \"%s\" (%s) is missing in YAML file", key.c_str(), description.c_str());
	}
}
