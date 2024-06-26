/**
 * Message printer (pulled from the SLIME assembler src) * 
 * 
 * (C) Ray Clemens 2023
 */

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "message.h"

msg_level_t msg_level;

/**
 * Print a message
 * 
 * @param type The prefix of the message
 * @param *filename The name of the file (NULL for no file)
 * @param linenum The line number (-1 for no file)
 * @param *fmt The vprintf format string
 * @param ... The args to the format string
 * @return The number of characters written
 */
int pmsg(msg_kind_t type, const char *file_name, ssize_t linenum, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    int len = 0;
    switch (type) {
    case M_ERRR:
        len += fprintf(stderr, "\x1b[1;31;40m[ERRR]\x1b[0m ");
        if (file_name) {
            len += fprintf(stderr, "'%s' ", file_name);
        }
        if (linenum >= 0) {
            len += fprintf(stderr, "line %ld: ", linenum);
        }
        len += vfprintf(stderr, fmt, args);
        break;
    case M_WARN:
        if (msg_level < ML_WARN) {
            break;
        }
        len += fprintf(stdout, "\x1b[1;33;40m[WARN]\x1b[0m ");
        if (file_name) {
            len += fprintf(stdout, "'%s' ", file_name);
        }
        if (linenum >= 0) {
            len += fprintf(stdout, "line %ld: ", linenum);
        }
        len += vfprintf(stdout, fmt, args);
        break;
    case M_INFO:
        if (msg_level < ML_INFO) {
            break;
        }
        len += fprintf(stdout, "\x1b[1;36;40m[INFO]\x1b[0m ");
        if (file_name) {
            len += fprintf(stdout, "'%s' ", file_name);
        }
        if (linenum >= 0) {
            len += fprintf(stdout, "line %ld: ", linenum);
        }
        len += vfprintf(stdout, fmt, args);
        break;
    case M_DBUG:
        if (msg_level < ML_DBUG) {
            break;
        }
        len += fprintf(stdout, "\x1b[1;35;40m[DBUG]\x1b[0m ");
        if (file_name) {
            len += fprintf(stdout, "'%s' ", file_name);
        }
        if (linenum >= 0) {
            len += fprintf(stdout, "line %ld: ", linenum);
        }
        len += vfprintf(stdout, fmt, args);
        break;
    default:
        len += fprintf(stdout, "\x1b[1;32;40m[????]\x1b[0m ");
        if (file_name) {
            len += fprintf(stdout, "'%s' ", file_name);
        }
        if (linenum >= 0) {
            len += fprintf(stdout, "line %ld: ", linenum);
        }
        len += vfprintf(stdout, fmt, args);
        break;
    }
    va_end(args);
    return len;
}

