#ifndef ARRAY_LIST_H_INCLUDED
#define ARRAY_LIST_H_INCLUDED
typedef struct array_list_t
{
    unsigned int size;
    unsigned int capacity;

    void *array_data;
}

/**
 * Initializes an array_list_t object by setting size to zero,
 * and allocating capacity for array_data
 * @param to_init pointer to array_list_t object to initialize
 **/
void ar_init( array_list_t* to_init );

#endif
