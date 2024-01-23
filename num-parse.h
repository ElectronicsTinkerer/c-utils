/**
 * Number parsing routines
 * Ray Clemens 2022, 2023
 * 
 * Updates:
 * 2020-10-??: Originally written for GSG V0
 * 2023-03-03: Update formating and add number base
 *             argument to int and uint parsers. 
 *             Also: add np_err2str() function
 */



#ifndef NUMBERPARSER_H
#define NUMBERPARSER_H

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>  // isspace()
#include <math.h>   // HUGE_VAL
#include <stdbool.h>
#include <stdio.h> // np_perr uses printf

// Remember to add these to np_err2str()
typedef enum NPError
{
    NP_SUCCESS = 0,
    NP_INVALID_STRING=1,
    NP_TOO_LARGE,
    NP_TOO_SMALL,
    NP_UNEXPECTED_NEGATIVE,
    NP_NON_NUMERIC
} NPError;

// Prototypes
NPError string_to_int(int *value, const char *string, int base);
NPError string_to_uint(int *value, const char *string, int base);
NPError string_to_double(double *value, const char *string);
bool is_whitespace(const char *string);
void np_perr(NPError);

#endif
