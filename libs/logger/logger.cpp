#include "logger.h"
#include <stdio.h>

namespace logger {

constexpr static char _level_always[] PROGMEM  = "[ALWAYS] ";
constexpr static char _level_fatal[] PROGMEM   = "[FATAL]  ";
constexpr static char _level_error[] PROGMEM   = "[ERROR]  ";
constexpr static char _level_warning[] PROGMEM = "[WARN]   ";
constexpr static char _level_info[] PROGMEM    = "[INFO]   ";
constexpr static char _level_debug[] PROGMEM   = "[DEBUG]  ";
constexpr static char _newline[] PROGMEM       = "\n";

constexpr static const char* _level_table[] PROGMEM = {_level_always,
                                                       _level_fatal,
                                                       _level_error,
                                                       _level_warning,
                                                       _level_info,
                                                       _level_debug};

void print_log(uint8_t level, const char* format PROGMEM, ...) {
    va_list args;
    va_start(args, format);

    printf_P((PGM_P)pgm_read_word(&(_level_table[level])));
    vfprintf_P(stdout, format, args);
    printf_P(_newline);

    va_end(args);
}

}  // namespace logger
