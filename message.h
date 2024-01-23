/**
 * Message printer (pulled from the SLIME assembler src)
 * 
 * (C) Ray Clemens 2023
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <unistd.h>

// Does not need to be in sync with msg_kind_t
typedef enum msg_level_t {
    ML_MINIMUM = 0,
    ML_ERRR = 0,
    ML_WARN,
    ML_INFO,
    ML_DBUG,
    ML_MAXIMUM
} msg_level_t;

#define ML_DEFAULT ML_INFO

typedef enum msg_kind_t {
    M_ERRR = 0,
    M_WARN,
    M_INFO,
    M_DBUG
} msg_kind_t;

int pmsg(msg_kind_t, const char*, ssize_t, const char *, ...);

#endif


