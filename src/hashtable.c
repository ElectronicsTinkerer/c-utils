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
 */

#define __HT_HT_C
#include "hashtable.h"
#undef __HT_HT_C

// Helper functions
static inline unsigned int _ht_compute_index(ht_t *table, unsigned long key);
// static inline float _ht_get_collision_average(ht_t *table);
static inline float _ht_get_load_factor(ht_t *table);
static void _ht_double_table(ht_t *table);
static void _ht_half_table(ht_t *table);


/**
 * Initialize a hashtable struct
 * 
 * @return 0 on success
 *         1 on failure (memory allocation failure)
 *         
 */
int ht_init(ht_t **table)
{
    *table = malloc(sizeof(**table));
    
    if (!*table) {
        return 1; // Unable to malloc memory
    }
    
    (*table)->arraySize = HT_INITIAL_SIZE;
    (*table)->currentLoadFactor = 0;
    (*table)->numberOfItemsInTable = 0;
    (*table)->numberOfSlotsUsed = 0;
    (*table)->table = malloc(sizeof(*((*table)->table)) * (*table)->arraySize);

    if ((*table)->table == NULL) {
        return 1;    // Unable to malloc memory
    }
    
    for (int i = 0; i < (*table)->arraySize; i++) {
        (*table)->table[i] = NULL;
    }
    return 0;
}


/**
 * Reset the hashtable to its original size and remove the elements from it<br>
 * Note: <code>free()</code>s all elements in table array!
 * 
 * @param table The hashtable to be cleared
 */
void ht_clear(ht_t *table)
{
    if (table != NULL)
    {
        // Free the elements in the table
        for (int i = 0; i < table->arraySize; i++)
        {
            if (table->table[i])
            {
                free(table->table[i]);
            }
        }
        free(table->table);

        // Create new table
        table->arraySize = HT_INITIAL_SIZE;
        table->table = malloc(sizeof(*(table->table)) * table->arraySize);
    }
}


/**
 * Add an element to the hashtable
 * Assumes that you have already malloc()'d the value pointer
 * 
 * @param table The HashTable to be operated upon
 * @param key The key associated with the provided value
 * @param value The value to be associated with the key
 */
void __ht_put(ht_t *table, unsigned long key, void *value)
{
    if (table != NULL)
    {
        unsigned int pointer = _ht_compute_index(table, key);

        ht_entry_t *item = malloc(sizeof(*item));
        
        item->next = NULL;
        item->value = value;
        item->key = key;

        // If this location in the table is empty, just add the node
        if (table->table[pointer] == NULL)
        {
            table->table[pointer] = item;
            table->numberOfSlotsUsed++;
        }
        else
        {
            ht_entry_t *node = table->table[pointer];
            int done = 0;
            while (node != NULL && !done)
            {
                if ((item->key) > (node->key) && node->next == NULL)
                {
                    node->next = item;
                    done = 1;
                }
                else if (node->next != NULL && (item->key) < (node->next->key) && (item->key) > (node->key))
                {
                    item->next = node->next;
                    node->next = item;
                    done = 1;
                }
                else if (item->key < node->key)
                {
                    item->next = node;
                    table->table[pointer] = item;
                    done = 1;
                }
                else if (item->key == node->key) // Item is the same as the node
                {
                    node->value = item->value;
                    free(item);
                    table->numberOfItemsInTable--; // Cancel out addition at end of function
                    done = 1;
                }
                node = node->next;
            }
        }

        // Update the number of items in the table and current load factor
        table->numberOfItemsInTable++;
        table->currentLoadFactor = _ht_get_load_factor(table);

        // Expand the table if needed
        if (table->currentLoadFactor - HT_DEFAULT_MAX_POSITIVE_LOAD_FACTOR_VARIANCE > HT_DEFAULT_LOAD_FACTOR)
        {
            _ht_double_table(table);
        }
    }
}


/**
 * Add an element to the hashtable
 * Assumes that you have already malloc()'d the value pointer
 * 
 * @param table The HashTable to be operated upon
 * @param key The key associated with the provided value
 * @param value The value to be associated with the key
 */
void __ht_sput(ht_t *table, char *key, void *value)
{
    if (table != NULL && key != NULL)
    {
        unsigned long keyi = ht_hash_string(key);
        __ht_put(table, keyi, value);
    }
}


/**
 * Get an element from the hashtable based on the specified key
 * 
 * @param table The table in which to search for the key
 * @param key The key corresponsing to the value that will be returned
 * @return The value corresponding to the key specified 
 *         (NULL if table is NULL or it element does not exist)
 */
void *__ht_get(ht_t *table, unsigned long key)
{
    if (table != NULL)
    {
        // Calculate the location in the table
        unsigned int hashValue = _ht_compute_index(table, key);
        ht_entry_t *node = table->table[hashValue];

        while (node != NULL)
        {
            if (node->key == key)
            {
                return node->value;
            }
            node = node->next;
        }
    }
    return NULL;
}


/**
 * Get an element from the hashtable based on the specified key
 * 
 * @param table The table in which to search for the key
 * @param key The key corresponsing to the value that will be returned
 * @return The value corresponding to the key specified 
 *         (NULL if table is NULL or it element does not exist)
 */
void *__ht_sget(ht_t *table, char *key)
{
    if (table != NULL && key != NULL)
    {
        // Calculate the location in the table
        unsigned long keyi = ht_hash_string(key);
        return __ht_get(table, keyi);
    }
    return NULL;
}


/**
 * Remove an item from the table
 * 
 * @param table The table from which to remove an element
 * @param key The key for the value to be removed
 * @return The value associated with the key, NULL if no such element exists
 */
void *__ht_remove(ht_t *table, unsigned long key)
{
    if (table != NULL)
    {
        // Calculate location in the table
        unsigned int hashValue = _ht_compute_index(table, key);
        ht_entry_t *node = table->table[hashValue];

        ht_entry_t *previousNode = NULL;

        while (node != NULL)
        {
            if (node->key == key)
            {
                if (previousNode == NULL)
                {
                    table->table[hashValue] = node->next;
                    if (node->next == NULL)
                    {
                        table->numberOfSlotsUsed--;
                    }
                }
                else
                {
                    previousNode->next = node->next;
                }

                // Update the number of items in the table and current load factor
                table->numberOfItemsInTable--;
                table->currentLoadFactor = ((float)table->numberOfSlotsUsed) / ((float)table->arraySize);
                
                // Shrink table if needed
                if (table->currentLoadFactor + HT_DEFAULT_MAX_NGATIVE_LOAD_FACTOR_VARIANCE < HT_DEFAULT_LOAD_FACTOR)
                {
                    _ht_half_table(table);
                }

                void *returnValue = node->value;
                /* free(node->value); */
                free(node);
                return returnValue; 
            }

            previousNode = node;
            node = node->next;
        }
    }
    return NULL; // Not Found
}


/**
 * Remove an item from the table
 * 
 * @param table The table from which to remove an element
 * @param key The key for the value to be removed
 * @return The value associated with the key, NULL if no such element exists
 */
void *__ht_sremove(ht_t *table, char *key)
{
    if (table != NULL)
    {
        // Calculate location in the table
        unsigned long keyi = ht_hash_string(key);
        return __ht_remove(table, keyi);
    }
    return NULL; // Not Found
}


/**
 * Search if a key exists
 * 
 * @param table The table in which to check for the key
 * @param key The key to search for
 * @return 1 if the key is found, 
 *         0 if not found or if table is NULL
 */
unsigned int ht_contains_key(ht_t *table, unsigned long key)
{
    if (table != NULL)
    {
        // Calculate location in the ht
        unsigned int hashValue = _ht_compute_index(table, key);
        ht_entry_t *node = table->table[hashValue];

        while (node != NULL)
        {
            if (node->key == key)
            {
                return 1;
            }
            node = node->next;
        }
    }
    return 0;
}


/**
 * Search if a key exists
 * 
 * @param table The table in which to check for the key
 * @param key The key to search for
 * @return 1 if the key is found, 
 *         0 if not found or if table is NULL
 */
unsigned int ht_contains_skey(ht_t *table, char *key)
{
    if (table != NULL)
    {
        // Calculate location in the ht
        unsigned long keyi = ht_hash_string(key);
        unsigned int hashValue = _ht_compute_index(table, keyi);
        ht_entry_t *node = table->table[hashValue];

        while (node != NULL)
        {
            if (node->key == keyi)
            {
                return 1;
            }
            node = node->next;
        }
    }
    return 0;
}


/**
 * Free a HashTable. 
 * @note This does NOT free() the values stored
 * @note The *table is set to NULL
 * 
 * @param table The table to free
 */
void ht_destroy(ht_t **table)
{
    if (table != NULL && *table != NULL)
    {
        ht_entry_t *node, *node2;
        for (unsigned int i = 0; i < (*table)->arraySize; i++)
        {
            if ((*table)->table[i] != NULL)
            {
                // If needed, free the collision list
                node = (*table)->table[i];
                while (node != NULL)
                {
                    node2 = node->next;
                    free(node);
                    node = node2;
                }
            }
        }
        free((*table)->table);
        free(*table);
        *table = NULL;
    }
}


/**
 * Returns if the hashtable contains 0 elements
 *
 * @param table The table to check if empty
 * @return True (1) if there are 0 elements in the HashTable, 
 *         False (0) if not empty or if NULL table
 */
unsigned int ht_is_empty(ht_t *table) 
{
    if (table != NULL)
    {
        return (table->arraySize) == 0; // FIXME: should this be numberOfItemsInTable ??
    }
    return 0;
}


/**
 * Returns the number of items in the hashtable
 * 
 * @param table The hashtable to retrieve the number of elements from
 * @return The number of elements in the table (0 if table is NULL)
 */
unsigned int ht_get_num_elements(ht_t *table)
{
    if (table != NULL)
    {
        return (table->numberOfItemsInTable);
    }
    return 0;
}


/**
 * Computes a hash value based off the input string
 * 
 * @param string The string to be hashed
 * @return The hashvalue of the string, 0 if string is NULL
 */
unsigned long ht_hash_string(const char *string)
{
    if (string != NULL)
    {
        // Algorithm based off:
        // http://www.cse.yorku.ca/~oz/hash.html
        // Apparently has "better distribution of the keys"
        int c;
        unsigned long hash = 0;
        while ((c = *string++))
        {
            hash += c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }
    return 0;
}


/**
 * Computes the pointer offset of a specified key into 
 * the hash table internal array
 * 
 * @param key The key to be "hashed"
 * @return The hash of the key 
 *         0 if the input table is NULL
 */
static unsigned int _ht_compute_index(ht_t *table, unsigned long key)
{
    if (table != NULL)
    {
        return key % (table->arraySize);
    }
    return 0;
}


// /**
//  * Get the average collision list length of the table
//  * 
//  * @param table The table to check collision list length average
//  * @return The average collision length of the collision lists in the table
//  *         -1 if the input table is NULL
//  */
// static float _ht_get_collision_average(ht_t *table)
// {
//     if (table != NULL)
//     {
//         return ((float)table->numberOfItemsInTable) / ((float)table->arraySize);
//     }
//     return -1;
// }


/**
 * Get the current table's load factor
 * 
 * @param table The table to check its load factor
 * @return The provided table's load factor
 *         -1 if the input table is NULL
 */
static float _ht_get_load_factor(ht_t *table)
{
    if (table != NULL)
    {
        return ((float)(table->numberOfItemsInTable)) / ((float)(table->arraySize));
    }
    return -1;
}


/**
 * Doubles the size of the array for the specified hashtable
 * 
 * @param table The table to be doubled in size
 */
static void _ht_double_table(ht_t *table)
{
    if (table != NULL)
    {
        // Keep a reference to the old table in the iterator
        ht_itr_t *itr = ht_create_iterator(table);
        table->arraySize = table->arraySize * 2;
        table->table = malloc(sizeof(ht_entry_t *) * table->arraySize);

        // Reset the values
        table->currentLoadFactor = 0;
        table->numberOfItemsInTable = 0;
        table->numberOfSlotsUsed = 0;

        for (unsigned int i = 0; i < table->arraySize; i++)
        {
            table->table[i] = NULL;
        }

        // Loop throught the original table and rehash them into the new table
        while (ht_iterator_has_next(itr))
        {
            ht_entry_t *node = __ht_iterator_next(itr);
            __ht_put(table, node->key, node->value);
            free(node);
        }

        // Free the old table
        free(itr->iteratorTable);

        ht_iterator_free(&itr);
    }
}


/**
 * Halve the size of the array for holding elements in the specified HashTable
 * 
 * @param table The table to be halved
 */
static void _ht_half_table(ht_t *table)
{
    if (table != NULL && (table->arraySize) >= (2 * HT_INITIAL_SIZE))
    {
        // Keep a reference to the old table in the iterator
        ht_itr_t *itr = ht_create_iterator(table);

        table->arraySize = (table->arraySize) / 2;
        table->table = malloc(sizeof(ht_entry_t *) * table->arraySize);

        // Reset the values
        table->currentLoadFactor = 0;
        table->numberOfItemsInTable = 0;
        table->numberOfSlotsUsed = 0;

        for (unsigned int i = 0; i < table->arraySize; i++)
        {
            table->table[i] = NULL;
        }

        // Loop through all the values in the original table, rehashing them into the new one
        while (ht_iterator_has_next(itr))
        {
            // TODO: Refactor this so that a the old nodes
            // are not reallocated everytime the HT changes
            // size. Would need an internal version of the
            // __ht_put() function.
            ht_entry_t *node = __ht_iterator_next(itr);
            __ht_put(table, node->key, node->value);
            free(node);
        }

        // Free the old table
        free(itr->iteratorTable);
        
        ht_iterator_free(&itr);
    }
}


/** 
 * Return a new HashTableIterator
 * 
 * @param table The HashTable to create an Iterator from
 * @return A reference to the new HashTableIterator
 *         NULL if the input table is NULL
 */
ht_itr_t *ht_create_iterator(ht_t * table)
{
    if (table != NULL)
    {
        ht_itr_t *itr = malloc(sizeof(*itr));
        itr->currentNode = table->table[0];
        itr->currentTableIndex = 0;
        itr->foundElements = 0;
        itr->totalElements = table->numberOfItemsInTable;
        itr->tableSize = table->arraySize;
        itr->iteratorTable = table->table;
        // memcpy(itr->iteratorTable, table->table, sizeof(ht_entry_t) * table->arraySize);
        return itr;
    }
    return NULL;
}


/**
 * Check if there are any more remaining elements in the ht_itr_t
 * 
 * @param itr The iterator to use for checking
 * @return 1 if there are more elements,
 *         0 if there are no remaining elements (or input table is null)
 */
int ht_iterator_has_next(ht_itr_t * itr)
{
    if (itr != NULL)
    {
        return ((itr->foundElements) < (itr->totalElements)) && 
                ((itr->currentTableIndex) < (itr->tableSize) ||
                ((itr->currentNode) != NULL && (itr->currentNode)->next != NULL));
    }
    return 0;
}


/**
 * Return the next element in the hashtable
 * 
 * @param itr The iterator to pull the next element from
 * @return The next element as we iterate through the hash table
 *         NULL if there is no remaining elements
 */
ht_entry_t *__ht_iterator_next(ht_itr_t * itr)
{
    if (itr != NULL)
    {
        if (ht_iterator_has_next(itr))
        {
            if (itr->currentNode != NULL)
            {
                ht_entry_t *returnNode = itr->currentNode;
                itr->currentNode = itr->currentNode->next;
                itr->foundElements++;
                return returnNode;
            }
            else
            {
                itr->currentNode = itr->iteratorTable[++(itr->currentTableIndex)];
                return __ht_iterator_next(itr);
            }
        }
        return NULL;
    }
    return NULL;
}


/**
 * Free a ht_itr_t
 * @note This sets the *itr pointer to NULL
 * 
 * @param itr The iterator to free
 */
void ht_iterator_free(ht_itr_t **itr)
{
    if (itr != NULL && *itr != NULL)
    {
        free(*itr);
        (*itr) = NULL;
    }
}

