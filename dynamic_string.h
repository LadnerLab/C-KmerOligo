#ifndef DYNAMIC_STRING_H_INCLUDED
#define DYNAMIC_STRING_H_INCLUDED
#define DEFAULT_LENGTH 256

typedef struct DynamicString
{
    int size;
    int capacity;
    char* data; 
} DynamicString;

int string_length( char* input );

void check_for_resize( DynamicString* input, char string_to_add[] );

void ds_init( DynamicString* input );

void ds_add( DynamicString* input, char string[] );

void ds_clear( DynamicString* input );

#endif
