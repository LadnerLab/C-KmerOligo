#include <stdlib.h>
#include "hash_table.h"

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
            total += input->key[ index ] << 3;
            index++;
        }
    return total;
}

void add_item( HashTable* table, HT_Entry* to_add )
{
    int item_index = generate_hash( to_add ) % table->capacity;

    if( table->table_data[ index ] == NULL )
        {
            table->table_data[ index ] = to_add;
        }
    else
        {

        }
          
}
