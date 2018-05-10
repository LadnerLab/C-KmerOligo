#ifndef ARRAY_LIST_H_INCLUDED
#define ARRAY_LIST_H_INCLUDED
typedef struct array_list_t
{
    unsigned int size;
    unsigned int capacity;

    void **array_data;
}

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

#endif
