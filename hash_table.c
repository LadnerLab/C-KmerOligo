#include <stdlib.h>
#include "hash_table.h"
#define HASH_NUMBER 2069

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

    table->table_data = malloc( sizeof( HT_Entry* ) * size ); 
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


int generate_hash( HT_Entry* input )
{
    int index = 0;
    int total = 0;
    while( input->key[ index ] )
        {
            total += ( input->key[ index ] * index + 1 ) % HASH_NUMBER;
            index++;
        }
    return total;
}

void ht_add( HashTable* table, char* to_add, int add_val )
{
    int item_index;
    int index;
    int quadratic_offset;

    HT_Entry *new_entry = malloc( sizeof( HT_Entry ) );

    new_entry->key = to_add;
    new_entry->value = add_val;

    item_index = generate_hash( new_entry ) % table->capacity;

    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = new_entry;
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
        }
          
}
