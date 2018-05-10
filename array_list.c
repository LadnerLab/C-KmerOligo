#include <stdlib.h>

#define DEFAULT_CAPACITY 256


int check_for_resize( array_list_t* array_check )
{
    void* new_data = NULL;
    size_t new_capacity;

    if( array_check->size + 1 >= array_check->capacity )
        {

            new_data = realloc( array_check->array_data,  );
            new_capacity = array_check->capacity * 2;

            if( new_data )
                {
                    array_check->array_data = new_data;
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
    if( capacity > 0 )
        {
            to_init->data = malloc( sizeof( void* ) * DEFAULT_CAPACITY ); 
            to_init->size = 0;
            to_init->capacity = DEFAULT_CAPACITY;
        }
}


void ar_clear( array_list_t* to_clear )
{
    unsigned int index;
    for( index = 0; index < to_clear->size; index++ )
        {
            free( to_clear->array_data[ index ] );
        }
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
