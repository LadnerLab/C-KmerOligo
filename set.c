#include <stdio.h>

#include "hash_table.h"

#define DEFAULT_SIZE 256


void set_init( set_t* to_init )
{
    ht_init( to_init->data, DEFAULT_SIZE );
}

void set_add( set_t* set_to_add, char* add_data )
{
    ht_add( set_to_add->data, add_data, NULL );
}

void set_remove( set_t* set_to_remove, char* remove_data )
{
    ht_remove( set_to_remove->data, remove_data );
}

void set_clear( set_t* set_to_clear )
{
    ht_clear( set_to_clear->data );
}


