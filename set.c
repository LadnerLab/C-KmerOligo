#include <stdio.h>
#include <stdlib.h>

#include "set.h"

#define DEFAULT_SIZE 256


void set_init( set_t* to_init )
{
    to_init->data = malloc( sizeof( hash_table_t ) );
    ht_init( to_init->data, DEFAULT_SIZE );
}

void set_add( set_t* set_to_add, char* add_data )
{
    ht_add( set_to_add->data, add_data, NULL );
}

int set_remove( set_t* set_to_remove, char* remove_data )
{
    return ht_delete( set_to_remove->data, remove_data );
}

void set_clear( set_t* set_to_clear )
{
    ht_clear( set_to_clear->data );
    free( set_to_clear );
}

void set_update( set_t* dest, set_t* source )
{
    HT_Entry** source_data = ht_get_items( source->data );
    unsigned int index;

    for( index = 0; index < source->data->size; index++ )
        {
            ht_add( dest->data, source_data[ index ]->key, NULL );
        }
}

void set_difference( set_t* dest, set_t* first, set_t* second )
{
    unsigned int index;
    HT_Entry** first_set_items = ht_get_items( first->data );

    for( index = 0; index < second->data->size; index++ )
        {
            if( find_item( second->data, first_set_items[ index ]->key ) == NULL )
                {
                    set_add( dest, first_set_items[ index ]->key );
                }
        }

}
