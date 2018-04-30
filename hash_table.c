#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define HASH_NUMBER 2069
#define ADDITIONAL_SPACE 25

// local method for calculating exponents
int int_to_pow( int base, int exponent )
{
    if( exponent == 0 )
        {
            return 1;
        }
    return base * int_to_pow( base, exponent - 1 );
}


void ht_init( HashTable* table, int size )
{
    int index;

    table->table_data = malloc( sizeof( HT_Entry* ) * ( size + ADDITIONAL_SPACE ) ); 
    table->capacity = size;

    for( index = 0; index < size; index++ )
        {
            table->table_data[ index ] = NULL;
        }

}

void ht_clear( HashTable* table )
{
    int index;
    for( index = 0; index < table->capacity; index++ )
        {
            free( table->table_data[ index ] );
        }

    free( table->table_data );
    free( table );
    table = NULL;
}


int generate_hash( char* key )
{
    int index = 0;
    int total = 0;
    while( key[ index ] )
        {
            total = total << 8;
            total += ( key[ index ] * index + 1 ) % HASH_NUMBER;
            index++;
        }
    return total;
}

int ht_add( HashTable* table, char* to_add, int add_val )
{
    int item_index;
    int index;
    int quadratic_offset;

    HT_Entry *new_entry = malloc( sizeof( HT_Entry ) );

    new_entry->key = to_add;
    new_entry->value = add_val;

    item_index = generate_hash( new_entry->key ) % table->capacity;

    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = new_entry;
            return 1;
        }
    else
        {
            quadratic_offset = 0;
            index = 1;

            do
                {
                    quadratic_offset += int_to_pow( index, 2 );
                    quadratic_offset %= table->capacity;

                    index++;
                }
            while( table->table_data[ quadratic_offset ] != NULL );

            table->table_data[ quadratic_offset ] = new_entry;

            return 1;
        }

    return 0;
}


int find_item_index( HashTable* table, char* in_key )
{
    int search_index = generate_hash( in_key ) % table->capacity;
    int quadratic_offset = 1;

    if( table->table_data[ search_index ] != NULL )
        {
            while( quadratic_offset < table->capacity )
                {
                    if( strcmp( table->table_data[ search_index ]->key, in_key ) == 0 )
                        {
                            return search_index;
                        }

                    search_index += int_to_pow( quadratic_offset, 2 );
                    search_index %= table->capacity;
                    quadratic_offset++;
                }
        }

    return ITEM_NOT_FOUND;
}


int *ht_find( HashTable* table, char* in_key )
{
    int index = find_item_index( table, in_key );

    if( index != ITEM_NOT_FOUND )
        {
            return &table->table_data[ index]->value;
        }
    return NULL;
}

int ht_delete( HashTable* table, char* in_key )
{
    int item_index = find_item_index( table, in_key );

    if( item_index != ITEM_NOT_FOUND )
        {
            free( table->table_data[ item_index ] );
            table->table_data[ item_index ] = NULL;

            return 1;
        }

    return 0;
}


