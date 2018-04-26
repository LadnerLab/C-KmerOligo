#include <stdlib.h>
#include "dynamic_string.h"
#define DEFAULT_LENGTH 256

int string_length( char[] input )
{
    int length = 0;

    while( *( input + length ) )
        {
            length++;
        }
    return length;
}

ds_init( DynamicString* input, char string[] )
{
    
}


