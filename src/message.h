/**
 * SLIME assembler
 * An improved HEIPA assembler replacement written in C
 * 
 * Message printer
 * 
 * (C) Ray Clemens 2023
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <unistd.h>

typedef enum msg_kind_t {
    M_ERRR = 0,
    M_WARN,
    M_INFO,
    M_DBUG
} msg_kind_t;

int pmsg(msg_kind_t, const char*, ssize_t, const char *, ...);

#endif


