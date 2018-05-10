#include <stdlib.h>

#include "array_list.h"

#define DEFAULT_CAPACITY 256


int check_for_resize( array_list_t* array_check )
{
    void* new_data = NULL;
    unsigned int new_capacity;

    if( array_check->size + 1 >= array_check->capacity )
        {

            new_capacity = array_check->capacity * 2;
            new_data = realloc( array_check->array_data, new_capacity );

            if( new_data )
                {
                    array_check->array_data = new_data;
                    array_check->capacity = new_capacity;
                }
            else
                {
                    return 0;
                }
        }
    return 1;
}
void ar_init( array_list_t* to_init )
{
    void* ptr = NULL;
    int index;
    to_init->array_data = malloc( sizeof( void* ) * DEFAULT_CAPACITY ); 

    for( index = 0; index < DEFAULT_CAPACITY; index++ )
        {
            to_init->array_data[ index ] = ptr;
        }
    
    to_init->size = 0;
    to_init->capacity = DEFAULT_CAPACITY;
}


void ar_clear( array_list_t* to_clear )
{
    free( to_clear->array_data );
    free( to_clear );
}


void *ar_get( array_list_t* to_get, unsigned int index )
{
    if( index < to_get->size )
        {
            return to_get->array_data[ index ];
        }
    return NULL;
}

void ar_add( array_list_t *to_add, void* new_data )
{
    check_for_resize( to_add );

    to_add->array_data[ to_add->size ] = new_data;
    to_add->size++;
}

void ar_set( array_list_t *to_set, unsigned int index, void* new_data )
{
    if( index < to_set->size )
        {
            to_set->array_data[ index ] = new_data;
        }
}
