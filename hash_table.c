#include "hash_table.h"

void ht_init( HashTable* table, int size )
{
    table->data = malloc( sizeof( HT_Entry ) * size ); 
    table->capacity = size;
}
