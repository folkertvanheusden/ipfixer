#include <string>
#include <vector>


std::string myformat(const char *const fmt, ...);

std::vector<std::string> split(const std::string & in_in, const std::string & splitter);

std::string str_tolower(std::string s);

std::string bin_to_text(const uint8_t *p, const size_t len);
