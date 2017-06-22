#ifndef LIBS_LOGGER_LOGGER_H_
#define LIBS_LOGGER_LOGGER_H_

#include <avr/pgmspace.h>

/*
 * Logging levels.
 * User should set LOG_LEVEL macro to one of those levels.
 */
#define LOG_LEVEL_NONE (-1)
#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_ALL 0xFF

/*
 * Every macro prints the log information with additional level message and
 * newline.
 * User should use of those macros:
 *
 * LOG_FATAL
 * LOG_ERROR
 * LOG_WARNING
 * LOG_INFO
 */

#if LOG_LEVEL < LOG_LEVEL_FATAL
#define LOG_FATAL(format, ...) logger::null_logger(__VA_ARGS__)
#else
#define LOG_FATAL(format, ...) logger::print_log(0, PSTR(format), ##__VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_ERROR
#define LOG_ERROR(...) logger::null_logger(__VA_ARGS__)
#else
#define LOG_ERROR(format, ...) logger::print_log(1, PSTR(format), ##__VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_WARNING
#define LOG_WARNING(...) logger::null_logger(__VA_ARGS__)
#else
#define LOG_WARNING(format, ...) \
    logger::print_log(2, PSTR(format), ##__VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_INFO
#define LOG_INFO(...) logger::null_logger(__VA_ARGS__)
#else
#define LOG_INFO(format, ...) logger::print_log(3, PSTR(format), ##__VA_ARGS__)
#endif


namespace logger {
void print_log(uint8_t, const char* PROGMEM, ...);
static constexpr void null_logger(...) {
}
}  // namespace logger

#endif  // LIBS_LOGGER_LOGGER_H_
