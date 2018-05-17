#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "set.h"
#include "hash_table.h"

#define CERTAINTY 0.99

#define DEFAULT_SIZE 256



int get_hash_count( int size, float expected_size )
{
    float k = ( size / expected_size ) * log( 2 );
    return (int) ceil( k );

}

int get_size( int items_to_store, float probability )
{
    float m = -( items_to_store * log( probability ) ) / ( pow( log( 2 ), 2 ) );

    return ceil( m );


}

void set_init( set_t* to_init, int items_count, float probability )
{
    int index;
    
    to_init->size = get_size( items_count, probability );
    to_init->bit_arr = malloc( sizeof( int* ) * to_init->size );
    to_init->hash_count = get_hash_count( to_init->size, (float) to_init->size / 2.0 );
    to_init->data = malloc( sizeof( hash_table_t ) );
    ht_init( to_init->data, items_count );

    for( index = 0; index < to_init->size; index++ )
        {
            to_init->bit_arr[ index ] = 0;
        }
}


void set_add( set_t* set_to_add, char* add_data )
{

    int index;
    int length = strlen( add_data );
    uint32_t current_element;
    for( index = 0; index < set_to_add->hash_count; index++ )
        {
            current_element = generate_hash( add_data, length, index + 1  ) % set_to_add->size;

            set_to_add->bit_arr[ current_element ] = 1;
        }
    ht_add( set_to_add->data, add_data, 0 );
}

int set_remove( set_t* set_to_remove, char* remove_data )
{
    /* return ht_delete( set_to_remove->data, remove_data ); */
    return NULL;
}

void set_clear( set_t* set_to_clear )
{
    ht_clear( set_to_clear->data );
    free( set_to_clear->bit_arr );
    free( set_to_clear );
}

int set_check( set_t* source, char* item )
{
    int index;
    int length = strlen( item );
    uint32_t current_element;

    for( index = 0; index < source->hash_count; index++ )
        {
            current_element = generate_hash( item, length, index + 1 ) % source->size;

            if( !source->bit_arr[ current_element ] )
                {
                    return 0;
                }
        }
    return 1;
}

void set_difference( set_t* dest, set_t* first, set_t* second )
{
    
    uint32_t index;
    for( index = 0; ( index < first->data->capacity ) && ( index < second->data->capacity ); index++ )
        {
            if( first->data->table_data[ index ] )
                {
                    if( !set_check( second, first->data->table_data[ index ]->key ) )
                        {
                            set_add( dest, first->data->table_data[ index ]->key );
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
