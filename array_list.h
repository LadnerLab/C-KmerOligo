#ifndef ARRAY_LIST_H_INCLUDED
#define ARRAY_LIST_H_INCLUDED
typedef struct array_list_t
{
    unsigned int size;
    unsigned int capacity;

    void *array_data;
} array_list_t;

/**
 * Initializes an array_list_t object by setting size to zero,
 * and allocating capacity for array_data
 * @param to_init pointer to array_list_t object to initialize
 **/
void ar_init( array_list_t* to_init );

/**
 * clears an arraylist object, frees each object in 
 * array_data, and then frees to_clear itself
 * @param to_clear pointer to array_list_t object to clear
 **/
void ar_clear( array_list_t* to_clear );

/**
 * Retrieves item from arraylist at index
 * @param to_get pointer to array_list_t to retrieve item from
 * @param index unsigned integer index at which to retrieve
 * @returns void pointer to object found at index, or null otherwise
 **/
void *ar_get( array_list_t* to_get, unsigned int index );

/**
 * Adds an item to specified array_list
 * Note: Automatically resizes the array_list if necessary
 * @param to_add pointer to array_list_t object to add to
 * @new_data void data to add to array_list
 **/
void ar_add( array_list_t *to_add, void* new_data );

#endif
