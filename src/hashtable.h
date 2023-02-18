/**
 * Simple hash table implementation in c
 * (C) Ray Clemens 2020 - 2023
 *
 * Updates:
 * 2020-10-xx: Created HT for GSG V0
 * 2023-02-08: Added function "generic" templating magic
 * 2023-02-09: Changed to macro cast wrappers
 * 2023-02-10: Fixed "definitely" and "indirectly" lost
 *             memory on table resize (valgrind)
 * 2023-02-17: Added __HT_HT_C #undef capability to allow
 *             for multiple hashtable generic types in
 *             the same compilation unit.
 */

#ifndef HT_H
#define HT_H

#include <string.h> // memcpy()
#include <stdlib.h>
#include <stddef.h> // NULL

#define HT_DEFAULT_MAX_POSITIVE_LOAD_FACTOR_VARIANCE 0.2
#define HT_DEFAULT_MAX_NGATIVE_LOAD_FACTOR_VARIANCE 0.5
#define HT_DEFAULT_LOAD_FACTOR 1.0
#define HT_INITIAL_SIZE 16

#define _HT_GLUE(x, y) x##y
#define HT_GLUE(x, y) _HT_GLUE(x, y)

typedef struct ht_t
{
    float currentLoadFactor;            // Load factor
    unsigned int numberOfItemsInTable;  // Number of items in the table
    unsigned int numberOfSlotsUsed;     // Number of table slots that have had data in them ("dirty slots")
    unsigned int arraySize;             // Current size of array to store elements
    struct ht_entry_t **table;   // The table in which to store the elements
} ht_t;

typedef struct ht_entry_t
{
    struct ht_entry_t *next;     // Using collision lists, this points to the next node in the list
    unsigned long key;
    void *value;
} ht_entry_t;

typedef struct ht_itr_t
{
    int currentTableIndex;
    int foundElements;
    int totalElements;
    int tableSize;
    struct ht_entry_t *currentNode;
    struct ht_entry_t **iteratorTable;
} ht_itr_t;


// Function Prototypes
int ht_init(ht_t **table);
void ht_clear(ht_t *table);
void __ht_put(ht_t *table, unsigned long key, void *value);
void __ht_sput(ht_t *table, char *key, void *value);
void *__ht_get(ht_t *table, unsigned long key);
void *__ht_sget(ht_t *table, char *key);
void *__ht_remove(ht_t *table, unsigned long key);
void *__ht_sremove(ht_t *table, char *key);
unsigned int ht_contains_key(ht_t *table, unsigned long key);
unsigned int ht_contains_skey(ht_t *table, char *key);
void ht_destroy(ht_t **table);
unsigned int ht_is_empty(ht_t *table);
unsigned int ht_get_num_elements(ht_t *table);
unsigned long ht_hash_string(const char *string);

ht_itr_t *ht_create_iterator(ht_t *table);
int ht_iterator_has_next(ht_itr_t *itr);
ht_entry_t *__ht_iterator_next(ht_itr_t *itr);
void ht_iterator_free(ht_itr_t **itr);

#endif

#ifndef __HT_HT_C

#ifndef HT_DATA_T
# error "Must define HT_DATA_T before including hashtable.h"
#endif

#define HT_ENTRY_DATA_T HT_GLUE(HT_DATA_T, _ht_entry_t)
typedef struct HT_ENTRY_DATA_T
{
    struct ht_entry_t *next;     // Using collision lists, this points to the next node in the list
    unsigned long key;
    HT_DATA_T *value;
} HT_ENTRY_DATA_T;


// "Macro Generic" templating wrappers
static inline void HT_GLUE(HT_DATA_T, _ht_put)(ht_t *t, unsigned long k, HT_DATA_T *v)
{
    __ht_put(t, k, (void*)v);
}

static inline void HT_GLUE(HT_DATA_T, _ht_sput)(ht_t *t, char *k, HT_DATA_T *v)
{
    __ht_sput(t, k, (void*)v);
}

static inline HT_DATA_T *HT_GLUE(HT_DATA_T, _ht_get)(ht_t *t, unsigned long k)
{
    return (HT_DATA_T*)__ht_get(t, k);
}

static inline HT_DATA_T *HT_GLUE(HT_DATA_T, _ht_sget)(ht_t *t, char *k)
{
    return (HT_DATA_T*)__ht_sget(t, k);
}

static inline HT_DATA_T *HT_GLUE(HT_DATA_T, _ht_remove)(ht_t *t, unsigned long k)
{
    return (HT_DATA_T*)__ht_remove(t, k);
}

static inline HT_DATA_T *HT_GLUE(HT_DATA_T, _ht_sremove)(ht_t *t, char *k)
{
    return (HT_DATA_T*)__ht_sremove(t, k);
}

static inline HT_ENTRY_DATA_T *HT_GLUE(HT_DATA_T, _ht_iterator_next)(ht_itr_t *itr)
{
    return (HT_ENTRY_DATA_T*)__ht_iterator_next(itr);
}


#undef HT_DATA_T
#undef HT_ENTRY_DATA_T

// The programmer must undef this if multiple
// hashtable types are to be defined within
// the same compilation unit. This prevents
// multiple definition warnings if multiple
// header files include the hashtable within
// the same unit with the same generic type.
#define __HT_HT_C

#endif


