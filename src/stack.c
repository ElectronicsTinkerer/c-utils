/**
 * Simple stack implementation in c
 * (C) Ray Clemens 2023
 *
 * Updates:
 * 2023-03-16: Initial creation
 * 
 * USAGE: see stack.h
 */

#define __STACK_STACK_C
#include "stack.h"
#undef __STACK_STACK_C

// Helper functions
static inline float _stack_get_load_factor(_stack_t *stack);
static bool _stack_double_stack(_stack_t *stack, size_t element_size);
static bool _stack_half_stack(_stack_t *stack, size_t element_size);


/**
 * Heap-allocate a new stack datastructure
 * 
 * @param **stack Pointer to the stack
 * @param element_size [internal use] Size of element stored in stack
 * @param initial_size Number of elements to store initially
 * @return true on success, false on failure (memory allocation failure)
 */
bool __stack_init(_stack_t **stack, size_t element_size, size_t initial_size)
{
    *stack = malloc(sizeof(**stack));
    if (!*stack) {
        return false;
    }
    (*stack)->table = malloc(sizeof(char) * element_size * initial_size);
    if (!((*stack)->table)) {
        free(*stack);
        return false;
    }
    (*stack)->current_load_factor = 0;
    (*stack)->number_of_items_in_table = 0;
    (*stack)->array_size = initial_size;
    (*stack)->min_array_size = initial_size;
    return true;
}


/**
 * Reset the stack to its initial size and remove all elements. 
 * Does not free any contained data.
 * 
 * @param *stack The stack to clear
 * @param element_size [internal use] Size of element stored in stack
 * @return true on success, false on failure (memory allocation failure)
 */
bool __stack_clear(_stack_t *stack, size_t element_size)
{
    char *tmp = realloc(stack->table, sizeof(char) * element_size * (stack->min_array_size));
    if (!tmp) {
        return false;
    }
    stack->table = tmp;
    stack->current_load_factor = 0;
    stack->number_of_items_in_table = 0;
    stack->array_size = stack->min_array_size;
    return true;
}


/**
 * Returns if there are any elements in the stack
 *
 * @param stack The stack to check if empty
 * @return True if there are 0 elements in the stack, 
 *         False if not empty or if NULL stack
 */
bool __stack_is_empty(_stack_t *stack) 
{
    if (stack != NULL)
    {
        return (stack->number_of_items_in_table) == 0;
    }
    return 0;
}


/**
 * Free a stack data structure
 * 
 * @param **stack The stack to free
 */
void __stack_destroy(_stack_t **stack)
{
    if (!stack || !*stack) {
        return;
    }
    free((*stack)->table);
    free(*stack);
}


/**
 * Get the element on the top of the stack
 * 
 * @param *stack The stack to look at
 * @param element_size [internal use] The size of an element stored in the stack
 * @param *element A pointer to the location to store the peek'd value
 * @return true on success, false on empty stack or null *element pointer
 */
bool __stack_peek(_stack_t *stack, size_t element_size, void *element)
{
    if (!element || __stack_is_empty(stack)) {
        return false;
    }
    memcpy(element, stack->table + (element_size * (stack->number_of_items_in_table - 1)), element_size);
    return true;
}


/**
 * Get the element at an index relative to the top of the stack
 * 
 * @param *stack The stack to look at
 * @param element_size [internal use] The size of an element stored in the stack
 * @param *element A pointer to the location to store the peek'd value
 * @param index The index, relative to the top of the stack, to retrieve.
 *              If 0 is given, this behaves the same as __stack_peek()
 * @return true on success, false on empty stack or null *element pointer or 
 *         index outside of stack size
 */
bool __stack_peeki(_stack_t *stack, size_t element_size, void *element, size_t index)
{
    if (!element || __stack_is_empty(stack) || index < 0 || index - 1 > stack->number_of_items_in_table) {
        return false;
    }
    memcpy(element, stack->table + (element_size * (stack->number_of_items_in_table - 1 - index)), element_size);
    return true;   
}


/**
 * Remove the top element on the stack and return its value
 * 
 * @param *stack The stack to remove the element from
 * @param element_size [internal use] The size of an element stored in the stack
 * @param *element A pointer to the location to store the pop'd value
 * @return true on success, false on memory error or empty stack or null *element pointer
 */
bool __stack_pop(_stack_t *stack, size_t element_size, void *element)
{
    if (!element || __stack_is_empty(stack)) {
        return false;
    }
    memcpy(element, stack->table + (element_size * (stack->number_of_items_in_table - 1)), element_size);

    --(stack->number_of_items_in_table);
    
    return _stack_half_stack(stack, element_size);
}


/**
 * Remove the top element on the stack
 * 
 * @param *stack The stack to remove the element from
 * @param element_size [internal use] The size of an element stored in the stack
 * @return true on success, false on memory error or empty stack or null *element pointer
 */
bool __stack_drop(_stack_t *stack, size_t element_size)
{
    if (__stack_is_empty(stack)) {
        return false;
    }

    --(stack->number_of_items_in_table);
    
    return _stack_half_stack(stack, element_size);
}


/**
 * Duplicate the item on the top of the stack
 * 
 * @param *stack The stack to perform the duplication on
 * @param element_size [internal use] The size of an element stored in the stack
 * @return true on success, false on memory error or empty stack or null *element pointer
 */
bool __stack_dup(_stack_t *stack, size_t element_size)
{
    char element[element_size];
    if (!__stack_peek(stack, element_size, &element)) {
        return false;
    }
    return __stack_push(stack, element_size, (void *)&element);
}


/**
 * If the stack contains at least two items, swap the top two items.
 * 
 * @param *stack The stack to perform the swap on
 * @param element_size [internal use] The size of an element stored in the stack
 * @return true on successful swap, false if the stack does not contain enough items
 */
bool __stack_swap(_stack_t *stack, size_t element_size)
{
    if (stack->number_of_items_in_table < 2) {
        return false;
    }
    char tmp_element[element_size];
    // Top item in stack
    memcpy(tmp_element, stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 1)), element_size);
    // Move 2nd-to-top item to top
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 1)),
           stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 2)),
           element_size);
    // Replace 2nd-to-top item
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 2)),
           tmp_element, element_size);
    return true;
}


/**
 * If the stack contains at least three items, rotate the top three items.
 * EX: if the stack contains 1,2,3<-top then after rotation, it will contain 2,3,1<-top
 * 
 * @param *stack The stack to perform the rotation on
 * @param element_size [internal use] The size of an element stored in the stack
 * @return true on successful rotation, false if the stack does not contain enough items
 */
bool __stack_rot(_stack_t *stack, size_t element_size)
{
    if (stack->number_of_items_in_table < 2) {
        return false;
    }
    char tmp_element[element_size];
    // Top item in stack
    memcpy(tmp_element, stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 1)), element_size);
    // Move 3rd-to-top item to top
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 1)),
           stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 3)),
           element_size);
    // Move 2rd-to-top item to 3rd-from-top
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 3)),
           stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 2)),
           element_size);
    // Replace 2nd-to-top item
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 2)),
           tmp_element, element_size);
    return true;
}


/**
 * Push an item onto the top of the stack
 * 
 * @param *stack The stack to perform the rotation on
 * @param element_size [internal use] The size of an element stored in the stack
 * @param *element The data to push onto the stack
 * @return true on success, false on failure (memory allocation failure)
 */
bool __stack_push(_stack_t *stack, size_t element_size, void *element)
{
    stack->number_of_items_in_table += 1;
    if (!_stack_double_stack(stack, element_size)) {
        stack->number_of_items_in_table -= 1;
        return false;
    }
    memcpy(stack->table + (sizeof(char) * element_size * (stack->number_of_items_in_table - 1)),
           element, element_size);
    return true;
}


/**
 * Returns the number of items in the stack
 * 
 * @param stack The stack to retrieve the number of elements from
 * @return The number of elements in the stack (0 if stack is NULL)
 */
size_t __stack_get_num_elements(_stack_t *stack)
{
    if (stack != NULL)
    {
        return (stack->number_of_items_in_table);
    }
    return 0;
}


/**
 * Get the current stack's load factor
 * 
 * @param stack The stack to check its load factor
 * @return The provided stack's load factor
 */
static float _stack_get_load_factor(_stack_t *stack)
{
    return ((float)(stack->number_of_items_in_table)) / ((float)(stack->array_size));
}


/**
 * Doubles the size of the array for the specified stack if the
 * stack's load factor is large enough
 * 
 * @param stack The stack to be doubled in size
 */
static bool _stack_double_stack(_stack_t *stack, size_t element_size)
{
    if (_stack_get_load_factor(stack) < STACK_DEFAULT_LOAD_FACTOR - STACK_DEFAULT_MAX_POSITIVE_LOAD_FACTOR_VARIANCE) {
        return true;
    }
    char *tmp  = realloc(stack->table, sizeof(char) * element_size * stack->array_size * 2);
    if (!tmp) {
        return false;
    }
    stack->table = tmp;
    stack->array_size *= 2;
    return true;
}


/**
 * Halve the size of the array for holding elements in the specified HashTable
 * 
 * @param table The table to be halved
 */
static bool _stack_half_stack(_stack_t *stack, size_t element_size)
{
    if (stack->array_size < 2 * stack->min_array_size ||
        _stack_get_load_factor(stack) > STACK_DEFAULT_LOAD_FACTOR - STACK_DEFAULT_MAX_NEGATIVE_LOAD_FACTOR_VARIANCE) {
        return true;
    }
    char *tmp  = realloc(stack->table, sizeof(char) * element_size * stack->array_size / 2);
    if (!tmp) {
        return false;
    }
    stack->table = tmp;
    stack->array_size /= 2;
    return true;
}

