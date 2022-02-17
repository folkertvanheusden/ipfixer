#include <map>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>


std::string      yaml_get_string   (const YAML::Node & node, const std::string & key, const std::string & description);
int              yaml_get_int      (const YAML::Node & node, const std::string & key, const std::string & description);
uint64_t         yaml_get_uint64_t (const YAML::Node & node, const std::string & key, const std::string & description, const bool units);
const YAML::Node yaml_get_yaml_node(const YAML::Node & node, const std::string & key, const std::string & description);
bool             yaml_get_bool     (const YAML::Node & node, const std::string & key, const std::string & description);
