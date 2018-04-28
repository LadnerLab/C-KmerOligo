#include "hash_table.h"

void ht_init( HashTable* table, int size )
{
    table->data = malloc( sizeof( HT_Entry ) * size ); 
    table->capacity = size;
}

void ht_clear( HashTable* table )
{
    int index = 0;
    while( table->table_data[ index ] ) 
        {
            free( table->table_data[ index ] );
            index++;
        }
    free( table );
    table = NULL;
}
