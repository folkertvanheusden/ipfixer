#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "logging.h"


std::string myformat(const char *const fmt, ...)
{
	char *buffer = nullptr;
        va_list ap;

        va_start(ap, fmt);

        if (vasprintf(&buffer, fmt, ap) == -1) {
		va_end(ap);

		dolog(ll_warning, "myformat: failed to convert string with format \"%s\"", fmt);

		return fmt;
	}

        va_end(ap);

	std::string result = buffer;
	free(buffer);

	return result;
}

std::vector<std::string> split(const std::string & in_in, const std::string & splitter)
{
	std::string in = in_in;

	std::vector<std::string> out;
	size_t splitter_size = splitter.size();

	for(;;)
	{
		size_t pos = in.find(splitter);
		if (pos == std::string::npos)
			break;

		std::string before = in.substr(0, pos);
		out.push_back(before);

		size_t bytes_left = in.size() - (pos + splitter_size);
		if (bytes_left == 0)
		{
			out.push_back("");
			return out;
		}

		in = in.substr(pos + splitter_size);
	}

	if (in.size() > 0)
		out.push_back(in);

	return out;
}

std::string str_tolower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });

	return s;
}

std::string bin_to_text(const uint8_t *p, const size_t len)
{
	char *temp = (char *)calloc(1, len * 6 + 1);

	for(size_t i=0; i<len; i++)
		snprintf(&temp[i * 3], 7, "%02x ", p[i]);

	std::string out = temp;

	free(temp);

	return out;
}
