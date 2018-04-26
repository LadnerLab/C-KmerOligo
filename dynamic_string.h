#ifndef DYNAMIC_STRING_H_INCLUDED
#define DYNAMIC_STRING_H_INCLUDED
#define DEFAULT_LENGTH 256

typedef struct DynamicString
{
    int size;
    char* data; 
} DynamicString;

int string_length( char* input );

void check_for_resize( DynamicString* input );

void ds_init( DynamicString* input, char input_str[] );

#endif
