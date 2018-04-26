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
void check_for_resize( DynamicString* input )
{
    int new_size;

    if( string_length( input->data ) > input->size + 10 )
        {
            new_size = input->size * 2;
            input->data = realloc( input->data, new_size );
            input->size = new_size;
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

void ds_init( DynamicString* input, char string[] )
{
    int length = string_length( string );

    input->data = malloc( sizeof( char ) * DEFAULT_LENGTH );

    if( length >= DEFAULT_LENGTH )
        {
            input->data = realloc( input->data, length * 2 );
        }
    input->size = length;
}




