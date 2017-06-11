#ifndef LIBS_LOGGER_LOGGER_H_
#define LIBS_LOGGER_LOGGER_H_

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3

#if LOG_LEVEL < LOG_LEVEL_FATAL
#define LOG_FATAL(...)  //
#else
#define LOG_FATAL(...)       \
    do {                     \
        printf("[FATAL] ");  \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
#endif

#if LOG_LEVEL < LOG_LEVEL_ERROR
#define LOG_ERROR(...)  //
#else
#define LOG_ERROR(...)       \
    do {                     \
        printf("[ERROR] ");  \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
#endif

#if LOG_LEVEL < LOG_LEVEL_WARNING
#define LOG_WARNING(...)  //
#else
#define LOG_WARNING(...)      \
    do {                      \
        printf("[WARNING] "); \
        printf(__VA_ARGS__);  \
        printf("\n");         \
    } while (0)
#endif

#if LOG_LEVEL < LOG_LEVEL_INFO
#define LOG_INFO(...)  //
#else
#define LOG_INFO(...)        \
    do {                     \
        printf("[INFO] ");   \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
#endif

#endif  // LIBS_LOGGER_LOGGER_H_
