#ifndef SET_H_INCLUDED
#define SET_H_INCLUDED
#include "hash_table.h"

typedef struct set_t
{
    hash_table_t* data;
} set_t;

void set_init( set_t* init );
void set_add( set_t* set_to_add, char* add_data );
void set_remove( set_t* set_to_remove, char* remove_data );
void set_clear( set_t* set_to_clear );
void set_update( set_t* dest, set_t* source );
void set_difference( set_t* dest, set_t* first, set_t* second );

#endif
