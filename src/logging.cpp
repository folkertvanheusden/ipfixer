#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <time.h>
#include <unistd.h>

#include "logging.h"
#include "str.h"
#include "time.h"


const char *logfile = strdup("/tmp/mystorage.log");
log_level_t log_level_file = ll_warning;
log_level_t log_level_screen = ll_warning;
static FILE *lfh = nullptr;
static int lf_uid = 0, lf_gid = 0;
static std::atomic_bool flush_thread_stop { false };
static std::thread *flush_thread { nullptr };

std::string ll_to_str(const log_level_t ll)
{
	if (ll == ll_debug)
		return "debug";

	if (ll == ll_info)
		return "info";

	if (ll == ll_warning)
		return "warning";

	if (ll == ll_error)
		return "error";

	dolog(ll_warning, "ll_to_str: log level %d is unknown", ll);

	return "debug";
}

log_level_t str_to_ll(const std::string & name)
{
	if (name == "debug")
		return ll_debug;

	if (name == "info")
		return ll_info;

	if (name == "warning")
		return ll_warning;

	if (name == "error")
		return ll_error;

	throw myformat("str_to_ll: \"%s\" is not recognized as a log-level", name.c_str());
}

static void stop_flush_thread()
{
	if (flush_thread) {
		flush_thread_stop = true;

		flush_thread->join();
		delete flush_thread;

		flush_thread = nullptr;
	}
}

void setlog(const char *lf, const log_level_t ll_file, const log_level_t ll_screen)
{
	if (lfh) {
		stop_flush_thread();

		fclose(lfh);
		lfh = nullptr;
	}

	if (!flush_thread) {
		flush_thread_stop = false;

		flush_thread = new std::thread([] {
				while(!flush_thread_stop) {
					sleep(1);
					fflush(lfh);
				}
			});
	}

	free(const_cast<char *>(logfile));

	logfile = strdup(lf);

	log_level_file = ll_file;
	log_level_screen = ll_screen;
}

void setloguid(const int uid, const int gid)
{
	lf_uid = uid;
	lf_gid = gid;
}

void closelog()
{
	stop_flush_thread();

	if (lfh) {
		fclose(lfh);
		lfh = nullptr;
	}

	free(const_cast<char *>(logfile));
}

void DOLOG(const log_level_t ll, const char *fmt, ...)
{
	if (!lfh) {
		lfh = fopen(logfile, "a+");
		if (!lfh) {
			fprintf(stderr, "Cannot access log-file %s: %s\n", logfile, strerror(errno));
			exit(1);
		}

		if (fchown(fileno(lfh), lf_uid, lf_gid) == -1)
			fprintf(stderr, "Cannot change logfile (%s) ownership: %s\n", logfile, strerror(errno));

		if (fcntl(fileno(lfh), F_SETFD, FD_CLOEXEC) == -1) {
			fprintf(stderr, "fcntl(FD_CLOEXEC): %s\n", strerror(errno));
			exit(1);
		}
	}

	uint64_t now = get_us();
	time_t t_now = now / 1000000;

	struct tm tm { 0 };
	if (!localtime_r(&t_now, &tm))
		fprintf(stderr, "localtime_r: %s\n", strerror(errno));

	char *ts_str = nullptr;

	const char *const ll_names[] = { "debug  ", "info   ", "warning", "error  " };

	asprintf(&ts_str, "%04d-%02d-%02d %02d:%02d:%02d.%06d |%d] %s ",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, int(now % 1000000),
			gettid(), ll_names[ll]);

	char *str = nullptr;

	va_list ap;
	va_start(ap, fmt);
	(void)vasprintf(&str, fmt, ap);
	va_end(ap);

	if (ll >= log_level_file) {
		fprintf(lfh, "%s%s\n", ts_str, str);

		if (ll >= ll_warning)
			fflush(lfh);
	}

	if (ll >= log_level_screen)
		printf("%s%s\n", ts_str, str);

	free(str);
	free(ts_str);
}

bool log_enabled(const log_level_t ll)
{
        return ll >= log_level_file || ll >= log_level_screen;
}
