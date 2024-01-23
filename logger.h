/**
 * Message printer (pulled from the SLIME assembler src)
 * 
 * (C) Ray Clemens 2023
 */

#ifndef LOGGER_H
#define LOGGER_H

typedef enum msg_kind_t {
    M_ERRR = 0,
    M_WARN,
    M_INFO,
    M_DBUG,
    M_SPCL
} msg_kind_t;

int pmsg(msg_kind_t, const char *, ...);

#endif


