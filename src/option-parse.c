/**
 * Argument parser implementation
 * (C) Ray Clemens 2023
 * 
 * Updates:
 * 2023-03-03: Created
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "option-parse.h"
#include "numberparser.h"

bool parse_args(option_entry_t *entries, int *argc, char *argv[], void (*help_func)())
{
    NPError np_err;
    option_entry_t *entry;
    char long_buf[OPTION_BUF_LEN] = "--";
    char short_buf[3] = "-_"; // '_' to be replaced in below loop
    
    while (*argc > 0) {

        // Loop over the different options available
        for (entry = entries; entry->type != OPT_EOL; ++entry) {

            // Check for help
            if (strcmp(*argv, "--help") == 0) {
                (help_func)();
                return false;
            }
            
            // Create the short and long argument string to compare agains
            short_buf[1] = entry->short_name;
            strcpy(long_buf+2, entry->full_name);

            if (entry->short_name != '\0' && strcmp(*argv, short_buf) == 0) {

                switch (entry->type) {
                case OPT_EOL:
                    break;
                    
                case OPT_TRUE:
                    *((bool*)entry->data_ptr) = true;
                    break;
                    
                case OPT_FALSE:
                    *((bool*)entry->data_ptr) = false;
                    break;
                    
                case OPT_BOOL:
                    if (strcmp(*(argv+1), "true") == 0 ||
                        strcmp(*(argv+1), "yes") == 0) {
                        
                        *((bool*)entry->data_ptr) = true;
                    }
                    else if (strcmp(*(argv+1), "false") == 0 ||
                             strcmp(*(argv+1), "no") == 0) {
                        
                        *((bool*)entry->data_ptr) = false;
                    }
                    else {
                        printf("Unknown boolean value: '%s' as argument to '%s'\n",
                               *(argv+1), *argv);
                        return false;
                    }
                    --*argc;
                    ++argv;
                    break;
                    
                case OPT_INT:
                    np_err = string_to_int(entry->data_ptr, *(argv+1), 10);
                    if (np_err != NP_SUCCESS) {
                        np_perr(np_err);
                        printf("Location: '%s' as argument to '%s'\n", *(argv+1), *argv);
                        return false;
                    }
                    --*argc;
                    ++argv;
                    break;
                    
                case OPT_UINT:
                    np_err = string_to_uint(entry->data_ptr, *(argv+1), 10);
                    if (np_err != NP_SUCCESS) {
                        np_perr(np_err);
                        printf("Location: '%s' as argument to '%s'\n", *(argv+1), *argv);
                        return false;
                    }
                    --*argc;
                    ++argv;
                    break;
                            
                case OPT_STR:
                    --*argc;
                    ++argv;
                    // NOTE: this does not check if the string is an argument ("-xx")
                    *((char**)entry->data_ptr) = *argv;
                    break;

                default:
                    printf("Internal error: unknown option type '%d'\n", entry->type);
                    // not fatal, keep going (next might be fatal though)
                    break;
                }
            }
        }
        
        --*argc;
        ++argv;
    }
    return true;
}

