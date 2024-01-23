/**
 * Number parsing routines
 * Ray Clemens 2022, 2023
 * 
 * Updates:
 * 2020-10-??: Originally written for GSG V0
 * 2023-03-03: Update formating and add number base
 *             argument to int and uint parsers
 * 2024-01-23: Rename numberparser -> num-parse
 */

#include "num-parse.h"


/**
 * Get an integer from a string. Only base 10 allowed.
 * Based on: https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 * 
 * @param value The location to store the parsed number from the input string
 * @param string The string to parse for an integer
 * @param base The number base
 * 
 * @return
 *      NP_SUCCESS          -   Returned if the string was successfully parsed
 *      NP_INVALID_STRING   -   Returned if the input string is NULL or 0-length
 *      NP_TOO_LARGE        -   Returned if the parsed value from the input string 
 *                              is larger than INT_MAX
 *      NP_TOO_SMALL        -   Returned if the parsed value from the input string
 *                              is smaller than INT_MIN
 */
NPError string_to_int(int *value, const char *string, int base)
{
    *value = 0;
   
    // Make sure that the string is "actually a string"
    if (is_whitespace(string)) {
        return NP_INVALID_STRING;
    }
    
    char *endPtr;
    errno = 0;
    long parsed_value = strtol(string, &endPtr, base);

    if (endPtr == string) {
        return NP_NON_NUMERIC;
    }

    *value = parsed_value;
    
    if (errno == ERANGE) {
        if (parsed_value == LONG_MAX) {
            return NP_TOO_LARGE;
        }
        return NP_TOO_SMALL;
    }
    
    // INT_MAX can == INT_LONG on some archetures, hence the separate "IF"
    if (parsed_value > INT_MAX) {
        errno = ERANGE;
        return NP_TOO_LARGE;
    }
    
    if (parsed_value < INT_MIN) {
        errno = ERANGE;
        return NP_TOO_SMALL;
    }

    return NP_SUCCESS;
}


/**
 * Get an unsigned integer from a string.
 * Note: Maximum value is INT_MAX, not UINT_MAX
 * 
 * @param value The location to store the parsed number from the input string
 * @param string The string to parse for an integer
 * @param base The number base
 * 
 * For return values, see string_to_int()
 * 
 * Additional return values:
 *      NP_UNEXPECTED_NEGATIVE  -   Returned if the parsed number was negative
 */
NPError string_to_uint(int *value, const char *string, int base)
{
    *value = 0;
    int error = string_to_int(value, string, base);
    if (!error && *value < 0) {
        return NP_UNEXPECTED_NEGATIVE;
    }
    return error; 
}


/**
 * Get a floating-point number from a string.
 * 
 * @param value The location to store the parsed number from the input string
 * @param string The string to parse for an double
 * @return
 *      NP_SUCCESS          -   Returned if the string was successfully parsed
 *      NP_INVALID_STRING   -   Returned if the input string is NULL or 0-length
 *      NP_TOO_LARGE        -   Returned if the parsed value from the input string 
 *                              is larger than INT_MAX
 *      NP_TOO_SMALL        -   Returned if the parsed value from the input string
 *                              is smaller than INT_MIN
 * 
 */
NPError string_to_double(double *value, const char *string)
{
    *value = 0.0;

    // Make sure that the string is "actually a string"
    if (is_whitespace(string))
        return NP_INVALID_STRING;

    char *endPtr;
    errno = 0;
    double parsed_value = strtod(string, &endPtr); // Base 10

    if (endPtr == string) {
        return NP_NON_NUMERIC;
    }

    *value = parsed_value;
    if (errno == ERANGE)
    {
        if (parsed_value == HUGE_VAL) {
            return NP_TOO_LARGE;
        }

        // HUGE_VAL has a sign
        return NP_TOO_SMALL;
    }

    return NP_SUCCESS;
}


/**
 * Check if a string contains only whitespace characters
 * 
 * @param string The string to check for whitespace status
 * @return false If the string is not blank
 *         true if the string is blank (or NULL)
 */
bool is_whitespace(const char *string)
{
    bool retVal = true;
    if (string != NULL)
    {
        int i = 0;
        while (retVal && string[i] != '\0')
        {
            if (!isspace(string[i]))
                retVal = false;

            i++;
        }
    }
    return retVal;
}


/**
 * Print a string corresponding to the error value
 * 
 * @param err The error number to print
 */
void np_perr(NPError err)
{
    printf("Number Parser: ");
    switch (err) {
    case NP_SUCCESS:
        printf("Success\n");
        break;
        
    case NP_INVALID_STRING:
        printf("Invalid string\n");
        break;
        
    case NP_TOO_LARGE:
        printf("Number too large (range error)\n");
        break;
        
    case NP_TOO_SMALL:
        printf("Number too small (range error)\n");
        break;
        
    case NP_UNEXPECTED_NEGATIVE:
        printf("Expected positive value, got negative\n");
        break;
        
    case NP_NON_NUMERIC:
        printf("Value not numberic in base\n");
        break;
        
    default:
        printf("Unknown error code: '%d'\n", err);
        break;
    }
}

