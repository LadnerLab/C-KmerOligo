#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "set.h"
#include "hash_table.h"

#define DEFAULT_SIZE 256

void set_init( set_t* to_init )
{
    to_init->data = malloc( sizeof( hash_table_t ) );
    ht_init( to_init->data, DEFAULT_SIZE );

}


void set_add( set_t* set_to_add, char* add_data )
{

   ht_add( set_to_add->data, add_data, 0 );
}

int set_remove( set_t* set_to_remove, char* remove_data )
{
    return ht_delete( set_to_remove->data, remove_data );
    return NULL;
}

void set_clear( set_t* set_to_clear )
{
    ht_clear( set_to_clear->data );
    free( set_to_clear );
}

void set_difference( set_t* first, set_t* second )
{
    uint32_t index;
    for( index = 0; ( index < first->data->capacity ); index++ )
        {
            if( first->data->table_data[ index ] )
                {
                    if( find_item( second->data, first->data->table_data[ index ]->key ) )
                        {
                            ht_delete( first->data, first->data->table_data[ index ]->key );
                        }
                }
        }
}

void set_add_all( set_t* dest, char** in_array, int num_elements )
{
    int index;

    for( index = 0; index < num_elements; index++ )
        {
            set_add( dest, in_array[ index ] );
        }
}
