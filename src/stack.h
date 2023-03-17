/**
 * Simple stack implementation in c
 * (C) Ray Clemens 2023
 *
 * Updates:
 * 2023-03-16: Initial creation
 * 
 * USAGE:
 * Define STACK_DATA_T as the data type to be stored in the stack structure.
 * Define STACK_DATA_NAME as the data name for the associated function calls.
 * **NOTE**: Do not enclose the above macros in parens! For example, if the 
 *           data type is char* , define the macros as char* , not (char*)
 */

#ifndef STACK_H
#define STACK_H

#include <stddef.h> // size_t
#include <stdbool.h>

#define STACK_DEFAULT_MAX_POSITIVE_LOAD_FACTOR_VARIANCE 0.2
#define STACK_DEFAULT_MAX_NEGATIVE_LOAD_FACTOR_VARIANCE 0.5
#define STACK_DEFAULT_LOAD_FACTOR 1.0

#define _STACK_GLUE(x, y) x##y
#define STACK_GLUE(x, y) _STACK_GLUE(x, y)

typedef struct _stack_t {
    float current_load_factor;             // Load factor
    unsigned int number_of_items_in_table; // Number of actual items currently in the internal array
    unsigned int array_size;               // Size of the internal array
    void *table;                          // Internal array
} _stack_t;

int __stack_init(_stack_t **stack, size_t element_size);
void __stack_clear(_stack_t *stack, size_t element_size);
bool __stack_is_empty(_stack_t *stack);
void __stack_destroy(_stack_t **stack, size_t element_size);
void *__stack_peek(_stack_t *stack, size_t element_size);
void *__stack_pop(_stack_t *stack, size_t element_size); // NOTE: TODO: stack pop needs to deallocate at the beginning of the function (not at the end) so that the returned pointer is still valid.
void __stack_dup(_stack_t *stack, size_t element_size);
void __stack_swap(_stack_t *stack, size_t element_size);
void __stack_rot(_stack_t *stack, size_t element_size);
void __stack_push(_stack_t *stack, size_t element_size, void *element);

#endif

#ifndef STACK_INITIAL_SIZE
#define STACK_INITIAL_SIZE 8
#endif


// Now on to the "type generic weirdness"
#ifndef __STACK_STACK_C

#if !defined(STACK_DATA_NAME) || !defined(STACK_DATA_T)
# error "Must define both STACK_DATA_NAME and STACK_DATA_T before including stack.h"
#endif

#define __STACK_T _STACK_GLUE(STACK_DATA_NAME, _stack_t)
typedef struct __STACK_T {
    float current_load_factor;             // Load factor
    unsigned int number_of_items_in_table; // Number of actual items currently in the internal array
    unsigned int array_size;               // Size of the internal array
    STACK_DATA_T *table;                   // Internal array
} __STACK_T;

// Function prototypes
static inline int STACK_GLUE(STACK_DATA_NAME, _stack_init)(__STACK_T **stack)
{
    return __stack_init((_stack_t**)stack, sizeof(STACK_DATA_T));
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_clear)(__STACK_T *stack)
{
    __stack_clear((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline bool STACK_GLUE(STACK_DATA_NAME, _stack_is_empty)(__STACK_T *stack)
{
    return __stack_is_empty((_stack_t*)stack);
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_destroy)(__STACK_T *stack)
{
    __stack_destroy((_stack_t**)stack, sizeof(STACK_DATA_T));
}

static inline STACK_DATA_T STACK_GLUE(STACK_DATA_NAME, _stack_peek)(__STACK_T *stack)
{
    return *(STACK_DATA_T *)__stack_peek((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline STACK_DATA_T STACK_GLUE(STACK_DATA_NAME, _stack_pop)(__STACK_T *stack)
{
    return *(STACK_DATA_T *)__stack_pop((_stack_t*)stack, sizeof(STACK_DATA_T));
}

// Basically an alias for stack_pop but with no return value
static inline void STACK_GLUE(STACK_DATA_NAME, _stack_drop)(__STACK_T *stack)
{
    __stack_pop((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_dup)(__STACK_T *stack)
{
    __stack_dup((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_swap)(__STACK_T *stack)
{
    __stack_swap((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_rot)(__STACK_T *stack)
{
    __stack_rot((_stack_t*)stack, sizeof(STACK_DATA_T));
}

static inline void STACK_GLUE(STACK_DATA_NAME, _stack_push)(__STACK_T *stack, STACK_DATA_T element)
{
    __stack_push((_stack_t*)stack, sizeof(STACK_DATA_T), (void*)&element);
}

#endif

