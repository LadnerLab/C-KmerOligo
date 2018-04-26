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

    if( input->capacity + 10 < input->size + add_length )
        {
            new_capacity = input->capacity * 2;
            input->data = (char *) realloc( input->data, new_capacity );
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

void ds_init( DynamicString* input, char string[] )
{
    int length = string_length( string );

    input->capacity = DEFAULT_LENGTH;
    input->data = (char*) malloc( sizeof( char ) * DEFAULT_LENGTH );

    if( length >= DEFAULT_LENGTH )
        {
            input->data = (char *) realloc( input->data, length * 2 );
        }
    input->size = length;
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
            *( input->data + size + input_length ) = string[ input_length ];
        }
    *( input->data + new_size ) = '\0';
}





