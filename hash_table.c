#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define HASH_NUMBER 2069
#define ADDITIONAL_SPACE 256

// local method for calculating exponents
int int_to_pow( int base, int exponent )
{
    if( exponent == 0 )
        {
            return 1;
        }
    return base * int_to_pow( base, exponent - 1 );
}


void ht_init( hash_table_t* table, int size )
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

void ht_clear( hash_table_t* table )
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
    int index = 0;
    int total = 0;
    while( *( key + index ) )
        {
            total += (int) key[ index ] << ( index + 1 );
            index++;
        }
    return total;
}

int ht_add( hash_table_t* table, char* to_add, void* add_val )
{
    int item_index;

    HT_Entry *new_entry = malloc( sizeof( HT_Entry ) );
    HT_Entry *current_node;

    char* key = malloc( strlen( to_add ) );
    key = memcpy( key, to_add, strlen( to_add ) );

    new_entry->key = key;
    new_entry->value = add_val;

    new_entry->next = NULL;
    new_entry->prev = NULL;

    item_index = generate_hash( to_add ) % table->capacity;

    // item not already in table
    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = new_entry;
            table->size++;
        }
    else
        {
            // item hash already in table
            current_node = table->table_data[ item_index ];
            while( current_node->next != NULL )
                {
                    current_node = current_node->next;
                }
            // we don't want to add duplicates
            if( strcmp( current_node->key, to_add ) == 0 )
                {
                    return 0;
                }
            current_node->next = new_entry;
            new_entry->prev = current_node;

            table->size++;
        }

    return 1;
}


HT_Entry* find_item( hash_table_t* table, char* in_key )
{
    int search_index = generate_hash( in_key ) % table->capacity;

    HT_Entry* current_node;

    if( table->table_data[ search_index ] != NULL )
        {
            current_node = table->table_data[ search_index ];
            while( strcmp( current_node->key, in_key ) != 0 ) 
                {
                    if( current_node->next == NULL )
                        {
                            return NULL;
                        }
                    current_node = current_node->next;
                }
            return current_node;
        }

    return NULL;
}


void *ht_find( hash_table_t* table, char* in_key )
{
    HT_Entry* found_item = find_item( table, in_key );

    if( found_item != NULL )
        {
            return found_item->value;
        }
    return NULL;
}

int ht_delete( hash_table_t* table, char* in_key )
{
    HT_Entry *found_node = find_item( table, in_key );
    int found_index = generate_hash( in_key ) % table->capacity;

    if( found_node != NULL )
        {
            if( found_node->prev != NULL )
                {
                    found_node->prev->next = found_node->next;
                }
            else if( found_node->next == NULL )
                {
                    *found_node = *found_node->next;
                }
            else
                {
                    table->table_data[ found_index ] = found_node->next;
                }


            free( found_node->key );
            free( found_node->value );
            free( found_node );
            return 1;
        }
    return 0;
}


HT_Entry **ht_get_items( hash_table_t* input )
{
    HT_Entry **output = NULL;
    HT_Entry* next_node;

    uint32_t input_index;
    uint32_t output_index;

    uint32_t capacity = input->capacity;

    if( input->size > 0 )
        {
            output = malloc( sizeof( HT_Entry* ) * input->size );
            output_index = 0;

            for( input_index = 0; input_index < capacity; input_index++ )
                {
                    if( input->table_data[ input_index ] )
                        {
                            output[ output_index ] = input->table_data[ input_index ];
                            next_node = input->table_data[ input_index ]->next;

                            output_index++;

                            while( next_node )
                                {
                                    output[ output_index ] = next_node;
                                    next_node = next_node->next;

                                    output_index++;
                                }
                        }
                }
        }
    return output;
}
