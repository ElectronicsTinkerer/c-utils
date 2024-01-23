/**
 * Message printer (pulled from the SLIME assembler src)
 * 
 * (C) Ray Clemens 2023
 */

#include <stdarg.h>
#include <stdio.h>

#include "logger.h"

/**
 * Print a message
 * 
 * @param type The prefix of the message
 * @param *fmt The vprintf format string
 * @param ... The args to the format string
 * @return The number of characters written
 */
int pmsg(msg_kind_t type, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    int len = 0;
    switch (type) {
    case M_ERRR:
        len += fprintf(stderr, "\x1b[1;31;40m[ERRR]\x1b[0m ");
        len += vfprintf(stderr, fmt, args);
        break;
    case M_WARN:
        len += fprintf(stdout, "\x1b[1;33;40m[WARN]\x1b[0m ");
        len += vfprintf(stdout, fmt, args);
        break;
    case M_INFO:
        len += fprintf(stdout, "\x1b[1;36;40m[INFO]\x1b[0m ");
        len += vfprintf(stdout, fmt, args);
        break;
    case M_DBUG:
        len += fprintf(stdout, "\x1b[1;35;40m[DBUG]\x1b[0m ");
        len += vfprintf(stdout, fmt, args);
        break;
    case M_SPCL:
        len += fprintf(stdout, "\x1b[1;34;40m[SPCL]\x1b[0m ");
        len += vfprintf(stdout, fmt, args);
        break;
    default:
        len += fprintf(stdout, "\x1b[1;32;40m[????]\x1b[0m ");
        len += vfprintf(stdout, fmt, args);
        break;
    }
    va_end(args);
    return len;
}
