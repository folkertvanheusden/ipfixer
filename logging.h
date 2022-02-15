#include <string>


typedef enum { ll_debug, ll_info, ll_warning, ll_error } log_level_t;

extern const char *logfile;
extern log_level_t log_level_file;
extern log_level_t log_level_screen;

log_level_t str_to_ll(const std::string & name);
std::string ll_to_str(const log_level_t ll);
void setlog(const char *lf, const log_level_t ll_file, const log_level_t ll_screen);
void setloguid(const int uid, const int gid);
void closelog();
void DOLOG(const log_level_t ll, const char *fmt, ...);
bool log_enabled(const log_level_t ll);

#define dolog(ll, ...) do {                                     \
        extern log_level_t log_level_file, log_level_screen;    \
                                                                \
        if (ll >= log_level_file || ll >= log_level_screen)     \
                DOLOG(ll __VA_OPT__(,) __VA_ARGS__);            \
        } while(0)
