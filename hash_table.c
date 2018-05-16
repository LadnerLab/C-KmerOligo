#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hash_table.h"

#define HASH_NUMBER 158341
#define ADDITIONAL_SPACE 256

void bt_add( HT_Entry* local_root, HT_Entry* new_entry );
HT_Entry* bt_search( HT_Entry* current_root, char *search_key );
HT_Entry* bt_remove_from_max( HT_Entry* local_root, HT_Entry* max_val );
void* bt_delete( HT_Entry* current_root, char *search_key );
HT_Entry* bt_delete_helper( HT_Entry* current_root, char *search_key );
void bt_clear( HT_Entry* current_node );

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
    uint32_t index;
    HT_Entry* current_node;
    for( index = 0; index < table->capacity; index++ )
        {
            current_node = table->table_data[ index ];
            if( current_node != NULL )
                {
                    bt_clear( current_node );
                }
            else
                {
                    // prevent double free of the root node
                    free( table->table_data[ index ] );
                }
            table->table_data[ index ] = NULL;

        }

    free( table->table_data );
    free( table );
    table = NULL;
}


uint32_t generate_hash( const void *key,  int len, uint32_t seed )
{
 const unsigned int m = 0xc6a4a793;

  const int r = 16;

  unsigned int h = seed ^ (len * m);

  //----------
  
  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    unsigned int k = *(unsigned int *)data;

    h += k;
    h *= m;
    h ^= h >> 16;

    data += 4;
    len -= 4;
  }
  
  //----------
  
  switch(len)
  {
  case 3:
    h += data[2] << 16;
  case 2:
    h += data[1] << 8;
  case 1:
    h += data[0];
    h *= m;
    h ^= h >> r;
  };
 
  //----------

  h *= m;
  h ^= h >> 10;
  h *= m;
  h ^= h >> 17;

  return h;
}

int ht_add( hash_table_t* table, char* to_add, void* add_val )
{
    uint32_t item_index;

    HT_Entry *new_entry = malloc( sizeof( HT_Entry ) );
    HT_Entry *current_node;

    char* key = to_add;
    int compare_val = 0;

    new_entry->key = key;
    new_entry->value = add_val;

    new_entry->left = NULL;
    new_entry->right = NULL;

    item_index = generate_hash( to_add, strlen( to_add), HASH_NUMBER ) % table->capacity;

    // item not already in table
    if( table->table_data[ item_index ] == NULL )
        {
            table->table_data[ item_index ] = new_entry;
        }
    else
        {
            // item hash already in table
            bt_add( table->table_data[ item_index ], new_entry );
        }

    table->size++;
    return 1;
}


HT_Entry* find_item( hash_table_t* table, char* in_key )
{
    uint32_t search_index = generate_hash( in_key, strlen( in_key ), HASH_NUMBER ) % table->capacity;


    HT_Entry* current_root;

    if( table->table_data[ search_index ] != NULL )
        {
            current_root = table->table_data[ search_index ];

            return bt_search( current_root, in_key );
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
    uint32_t found_index = generate_hash( in_key, strlen( in_key ), HASH_NUMBER ) %
                           table->capacity;

    if( found_node != NULL )
        {
            if( table->table_data[ found_index ] == found_node )
                {
                    table->table_data[ found_index ] = found_node->next;
                }
            if( found_node->next != NULL )
                {
                    found_node->next->prev = found_node->prev;
                }
            if( found_node->prev != NULL )
                {
                    found_node->prev->next = found_node->next;
                }


            free( found_node->value );
            free( found_node );

            table->size -= 1;

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

void bt_add( HT_Entry* local_root, HT_Entry* new_entry )
{
    int compare_val = strcmp( local_root->key, new_entry->key );

    if( compare_val > 0 )
        {
            if( local_root->left == NULL )
                {
                    local_root->left = new_entry;
                }
            else
                {
                    bt_add( local_root->left, new_entry );
                }
        }
    else if( compare_val < 0 )
        {
            if( local_root->right == NULL )
                {
                    local_root->right = new_entry;
                }
            else
                {
                    bt_add( local_root->left, new_entry );
                }
        }
    else
        {
            free( local_root->value );
            local_root->value = new_entry->value;
            free( new_entry );
        }
    
}


HT_Entry* bt_search( HT_Entry* current_root, char *search_key )
{
    int compare_val = strcmp( current_root->key, search_key )
        if( compare_val > 0 )
            {
                return bt_search( current_root->left, search_key );
            }
        else if( compare_val < 0 )
            {
                return bt_search( current_root->right, search_key );
            }
        else
            {
                return current_root;
            }
}


void* bt_delete( HT_Entry* current_root, char *search_key )
{
    HT_Entry* found_data = bt_search( current_root, search_key )
    if( found_data != NULL )
        {
            bt_delete_helper( current_root, search_key );
        }
    return found_data;
}

HT_Entry* bt_remove_from_max( HT_Entry* local_root, HT_Entry* max_val )
{
    if( max_val->right != NULL )
        {
            return bt_remove_from_max( max_val, max_val->right );
        }
    return max_val;
    local_root->right = NULL;
}

HT_Entry* bt_delete_helper( HT_Entry* current_root, char *search_key )
{
    int compare_val;
    
    if( current_root != NULL )
        {
            compare_val = strcmp( current_root->key, search_key );

            if( compare_val > 0 )
                {
                    current_root->left = bt_delete( current_root->left, search_key );
                }
            else if( compare_val < 0 )
                    {
                    current_root->right = bt_delete( current_root->right, search_key );
                    }
            else
                {
                    if( current_root->left == NULL && current_root->right == NULL )
                        {
                            free( current_root );
                            current_root = NULL;
                        }
                    else if( current_root->left == NULL )
                        {
                            free( current_root );
                            current_root = current_root->right;
                        }
                    else if( current_root->right == NULL )
                        {
                            free( current_root );
                            current_root = current_root->left;
                        }
                    else
                        {
                            if( current_root->left->right == NULL )
                                {
                                    current_root->value = current_root->left->value;
                                    free( current_root->left );
                                    current_root->left = current_root->left->left;
                                }
                            else
                                {
                                    current_root->value =
                                        bt_remove_from_max( current_root,
                                                            current_root->left
                                                            )->value;
                                }
                        }
                }
            
            return current_root;
        }
}

void bt_clear( HT_Entry* current_node )
{
    if( curent_node != NULL )
        {
            bt_clear( current_node->left );
            bt_clear( current_node->right );

            free( current_node->value );
            free( current_node );
            current_node = NULL;
        }
}
