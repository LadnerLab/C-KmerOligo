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
    table->table_data = malloc( sizeof( HT_Entry ) * size ); 
    table->capacity = size;
}

void ht_clear( HashTable* table )
{
    free( table->table_data->value );
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

void add_item( HashTable* table, HT_Entry* to_add )
{
    int item_index = generate_hash( to_add ) % table->capacity;

    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = to_add;
        }
          
}
