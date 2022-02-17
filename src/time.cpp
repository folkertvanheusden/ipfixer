#include <stdint.h>
#include <time.h>

#include "error.h"


uint64_t get_ms()
{
	struct timespec tv;

	if (clock_gettime(CLOCK_REALTIME, &tv) == -1)
		error_exit(true, "get_us: failed retrieving time");

	return uint64_t(tv.tv_sec) * uint64_t(1000) + uint64_t(tv.tv_nsec / 1000000);
}

uint64_t get_us()
{
	struct timespec tv;

	if (clock_gettime(CLOCK_REALTIME, &tv) == -1)
		error_exit(true, "get_us: failed retrieving time");

	return uint64_t(tv.tv_sec) * uint64_t(1000 * 1000) + uint64_t(tv.tv_nsec / 1000);
}
