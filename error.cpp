#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void error_exit(const bool se, const char *format, ...)
{
	int e = errno;
	va_list ap;

	va_start(ap, format);

	char *temp = NULL;
	if (vasprintf(&temp, format, ap) == -1)
		puts(format);  // last resort

	va_end(ap);

	fprintf(stderr, "%s\n", temp);

	if (se && e)
		fprintf(stderr, "errno: %d (%s)\n", e, strerror(e));

	free(temp);

	exit(EXIT_FAILURE);
}
