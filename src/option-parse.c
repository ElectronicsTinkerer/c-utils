/**
 * Argument parser implementation
 * (C) Ray Clemens 2023
 * 
 * Updates:
 * 2023-03-03: Created
 * 2023-03-19: Fix sefgault on NULL *help_func
 * 2023-04-01: Add arg/flag table print (opt_help())
 * 2023-04-01: Add "Option Section Header"s
 * 2023-04-01: Added check for unknown option flag
 * 2023-04-06: Fix long filename check in parse_args
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "option-parse.h"
#include "numberparser.h"

char *option_type_strs[] = {
    "EOL",
    "\0",
    "\0",
    "\0",
    "bool",
    "int",
    "uint",
    "str",
    "filename",
    "dir"
};

bool parse_args(option_entry_t *entries, int *argc, char *argv[], void (*help_func)())
{
    NPError np_err;
    option_entry_t *entry;
    char long_buf[OPTION_BUF_LEN] = "--";
    char short_buf[3] = "-_"; // '_' to be replaced in below loop
    bool flag_exists;
    bool encountered_error = false;
    
    while (*argc > 0) {

        // Loop over the different options available
        flag_exists = false;
        for (entry = entries; entry->type != OPT_EOL; ++entry) {

            // Check for help
            if (strcmp(*argv, "--help") == 0) {
                if (help_func != NULL) {
                    (help_func)();
                }
                else {
                   opt_help(entries);
                }
                return false;
            }

            // Check for "--" since this will terminate option parsing
            if (strcmp(*argv, "--") == 0) {
                return true;
            }
            
            // Create the short and long argument string to compare agains
            short_buf[1] = entry->short_name;
            strcpy(long_buf+2, entry->full_name);

            if ((entry->short_name != '\0' && strcmp(*argv, short_buf) == 0) ||
                (entry->full_name[0] != '\0' && strcmp(*argv, long_buf) == 0)) {

                flag_exists = true;

                switch (entry->type) {
                case OPT_SECT_HDR:
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
                            
                case OPT_FILENAME:
                case OPT_DIR:
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

        if (!flag_exists) {
            printf("Option error: '%s' (unknown)\n", *argv);
            encountered_error = true;
        }
        
        --*argc;
        ++argv;
    }

    return !encountered_error;
}

/**
 * Use the information in the option entries array to generate and
 * print a USAGE/help menu to STDOUT
 * 
 * @param *entries 
 */
void opt_help(option_entry_t *entries)
{
    size_t max_arg_len = 0;
    size_t current_len = 0;
    bool has_both = true;
    option_entry_t *entry;
    char *desc_tok;
    char desc_tmp[1024];
    bool in_line_desc;

    // Figure out how wide the args flags are
    for (entry = entries; entry->type != OPT_EOL; ++entry) {
        if (entry->short_name != '\0') {
            current_len = 3;
            has_both = true;
        }
        else {
            current_len = 0;
            has_both = false;
        }

        if (entry->full_name[0] != '\0') {
            current_len += strnlen(entry->full_name, OPTION_MAX_LEN) + 3;
        }
        else {
            has_both = false;
        }

        // Account for "types"
        if (option_type_strs[entry->type][0] != '\0') {
            current_len += strlen(option_type_strs[entry->type]);
            // Enclosed in <> plus a leading space
            current_len += 3;
        }

        // Account for the ", " between the short and long args
        if (has_both) {
            current_len += 2;
        }
        
        if (current_len > max_arg_len) {
            max_arg_len = current_len;
        }
    }

    printf("USAGE:\n");

    // Now actually print out the flags/args
    for (entry = entries; entry->type != OPT_EOL; ++entry) {

        // Check for section headers
        if (entry->type == OPT_SECT_HDR) {
            printf("\n=== %s ===\n", entry->flag_description);
            continue;
        }
        
        
        // Print short flag
        if (entry->short_name != '\0') {
            printf(" -%c", entry->short_name);
            current_len = 3;
        }
        else {
            current_len = 0;
        }

        // Print long flag
        if (entry->full_name[0] != '\0') {
            if (entry->short_name != '\0') {
                printf(",");
                current_len += 1;
            }
            printf(" --%s", entry->full_name);
            current_len += strnlen(entry->full_name, OPTION_MAX_LEN) + 3;
        }

        // Print argument to flag
        if (option_type_strs[entry->type][0] != '\0') {
            printf(" <%s>", option_type_strs[entry->type]);
            current_len += strlen(option_type_strs[entry->type]) + 3;
        }

        if (entry->flag_description != NULL) {
            // Print the dots between the flags and the descriptions
            if (current_len < max_arg_len - 1) {
                putc(' ', stdout);
                ++current_len;
                while (current_len < max_arg_len - 1) {
                    putc('.', stdout);
                    ++current_len;
                }
            }

            // Print the description
            strncpy(desc_tmp, entry->flag_description, 1023);
            desc_tmp[1023] = '\0';
            desc_tok = strtok(desc_tmp, "\n");
            in_line_desc = true;
            do {
                // Indent "wrapped" lines
                if (!in_line_desc) {
                    for (current_len = 0; current_len < max_arg_len - 1; ++current_len) {
                        putc(' ', stdout);
                    }
                }
                else {
                    in_line_desc = false;
                }
                printf(" %s\n", desc_tok);
            } while ((desc_tok = strtok(NULL, "\n")) != NULL);
        }
        else {
            putc('\n', stdout);
        }
    }

}

