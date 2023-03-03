/**
 * Argument parser implementation
 * (C) Ray Clemens 2023
 * 
 * Updates:
 * 2023-03-03: Created
 */

#ifndef OPTION_PARSE_H
#define OPTION_PARSE_H

#define OPTION_MAX_LEN 32
#define OPTION_BUF_LEN (OPTION_MAX_LEN + 2)

typedef enum option_type_t {
    OPT_EOL,   // End of list
    OPT_TRUE,  // Set var to true if flag exists
    OPT_FALSE, // Set var to false if flag exists
    OPT_BOOL,
    OPT_INT,
    OPT_UINT,
    OPT_STR
} option_type_t;

typedef struct option_entry_t {
    char short_name;
    char full_name[32];
    option_type_t type;
    void *data_ptr;
} option_entry_t;

#define OPTION_END_LIST {'\0', "", OPT_EOL, NULL}

bool parse_args(option_entry_t *entries, int *argc, char *argv[], void (*help_func)());

#endif

