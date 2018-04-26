#include <stdlib.h>
#include <stdbool.h>
#include "dynamic_string.h"
#define DEFAULT_LENGTH 256

/**
 * Checks whether or not a DynamicString object needs to be resized
 * if it does, automatically reallocs the object's string buffer
 * at twice its size
 * 
 * @param input DynamicString input to be tested 
 **/
void check_for_resize( DynamicString* input, char string_to_add[] )
{
    int new_capacity;
    int add_length = string_length( string_to_add );

    if( input->capacity + DEFAULT_LENGTH < input->size + add_length )
        {
            new_capacity = ( input->capacity ) + DEFAULT_LENGTH;
            input->data =  realloc( input->data, new_capacity );
            input->capacity = new_capacity;
        }
}

int string_length( char* input )
{
    int length = 0;

    while( *( input + length ) )
        {
            length++;
        }
    return length;
}

void ds_init( DynamicString* input )
{
    input->capacity = DEFAULT_LENGTH;
    input->data = malloc( DEFAULT_LENGTH );
    input->size = 0;
}


void ds_add( DynamicString* input, char string[] )
{
    int size = input->size;
    int input_length = string_length( string );
    int new_size = input->size + input_length;

    int index = 0;

    check_for_resize( input, string );

    for( index = 0; index < input_length; index++ )
        {
            *( input->data + size + index ) = string[ index ];
        }

    *( input->data + new_size ) = '\0';

    input->size = new_size;
}

void ds_clear( DynamicString* input )
{
    free( input->data );
}





