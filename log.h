#ifndef _MUS_LOG_H
#define _MUS_LOG_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

typedef enum {MUS_INFO, MUS_WARNING, MUS_ERROR} MUS_LogType;

static inline void MUS_print_log(MUS_LogType level, const char* fmt, ...) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    printf("[%d/%02d/%02d@%02d:%02d:%02d] ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    switch (level) {
        case MUS_INFO: printf("[INFO] "); break;
        case MUS_WARNING: printf("\x1B[33m[WARNING] "); break;
        case MUS_ERROR: printf("\x1B[31;1;4m[ERROR] "); break;
    }
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\x1B[0m\n");
}

static inline void MUS_print_log_to_file(const char* file_path, MUS_LogType level, const char* fmt, ...) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    FILE* file=fopen(file_path, "a");
    if (file == NULL) return;
    fprintf(file, "[%d/%02d/%02d@%02d:%02d:%02d] ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    switch (level) {
        case MUS_INFO: fprintf(file, "[INFO] "); break;
        case MUS_WARNING: fprintf(file, "[WARNING] "); break;
        case MUS_ERROR: fprintf(file, "[ERROR] "); break;
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
    fprintf(file, "\n");
    fclose(file);
}

#endif //_MUS_LOG_H

#ifndef MUS_LOG_STRIP_PREFIX_GUARD_
#define MUS_LOG_STRIP_PREFIX_GUARD_
    #ifndef MUS_DONT_STRIP_PREFIX
        #define INFO MUS_INFO
        #define WARNING MUS_WARNING
        #define ERROR MUS_ERROR
        #define LogType MUS_LogType
        #define print_log MUS_print_log
        #define print_log_to_file MUS_print_log_to_file
    #endif
#endif // MUS_LOG_STRIP_PREFIX_GUARD_