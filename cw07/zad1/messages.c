#include <stdarg.h>
#include <stdio.h>

void log_message (const char *format, ...) {
    va_list arg;
    int done;

    va_start(arg, format);
    va_end (arg);
}
