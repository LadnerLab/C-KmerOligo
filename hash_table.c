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
    table->size = 0;

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
            table->table_data[ index ] = NULL;
        }

    free( table->table_data );
    free( table );
    table = NULL;
}


int generate_hash( char* key )
{
    unsigned int index = 0;
    int total;
    while( *( key + index ) )
        {
            total += (int) key[ index ] << ( index + 1 );
            index++;
        }
    return total;
}

int ht_add( HashTable* table, char* to_add, void* add_val )
{
    int item_index;

    HT_Entry *new_entry = malloc( sizeof( HT_Entry ) );
    HT_Entry *current_node;

    new_entry->key = to_add;
    new_entry->value = add_val;
    new_entry->next = NULL;

    item_index = generate_hash( new_entry->key ) % table->capacity;

    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = new_entry;
            return 1;
        }
    else
        {
            current_node = table->table_data[ item_index ];
            while( current_node->next != NULL )
                {
                    // we don't want to add duplicates
                    if( strcmp( current_node->key, to_add ) == 0 )
                        {
                            return 0;
                        }

                    current_node = current_node->next;
                }
            current_node->next = new_entry;

            table->size++;
            return 1;
        }

    return 0;
}


int find_item_index( HashTable* table, char* in_key )
{
    int search_index = generate_hash( in_key ) % table->capacity;

    HT_Entry* current_node;

    if( table->table_data[ search_index ] != NULL )
        {
            current_node = table->table_data[ search_index ];
            while( strcmp( table->table_data[ search_index ]->key, in_key ) != 0 ) 
                {
                    if( current_node->next == NULL )
                        {
                            return ITEM_NOT_FOUND;
                        }
                    current_node = current_node->next;
                }
        }

    return ITEM_NOT_FOUND;
}


void *ht_find( HashTable* table, char* in_key )
{
    int index = find_item_index( table, in_key );

    if( index != ITEM_NOT_FOUND )
        {
            return &table->table_data[ index ]->value;
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


