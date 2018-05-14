#ifndef SET_H_INCLUDED
#define SET_H_INCLUDED

typedef struct set_t
{
    hash_table_t* data;
} set_t;

void set_init( set_t* init );
void set_add( set_t* set_to_add, char* add_data );
void set_remove( set_t* set_to_remove, char* remove_data );
void set_clear( set_t* set_to_clear );

#endif
