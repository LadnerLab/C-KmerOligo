#ifndef DYNAMIC_STRING_H_INCLUDED
#define DYNAMIC_STRING_H_INCLUDED
#define DEFAULT_LENGTH 256

typedef struct DynamicString
{
    int size;
    int capacity;
    char* data; 
} DynamicString;

/**
 * Calculates the length of a string
 * @param input character pointer to first character of string
 * @return integer length of string
 *
 **/
int string_length( char* input );

/**
 * Checks whether or not a DynamicString object needs to be resized
 * if it does, automatically reallocs the object's string buffer
 * to have DEFAULt_LENGTH more characters
 * 
 * @param input DynamicString input to be tested 
 **/
void check_for_resize( DynamicString* input, char string_to_add[] );


/**
 * Initializes input DynamicString struct instance
 * Note: mallocs memory on the heap for object's data
 * @param input pointer to DynamicString instance to be initialized
 **/
void ds_init( DynamicString* input );

/**
 * Adds new string to a dynamicString object
 * Note: Uses check_for_resize method to see if the object should
 * be given more memory
 * @param input DynamicString object to be added to
 * @param string[] string array to append to input
 **/
void ds_add( DynamicString* input, char string[] );

/**
 * clears a DynamicString struct instance
 * Note: Frees both the object itself and the data
 * it contains
 * @param input pointer to DynamicString instance to be freed
 **/
void ds_clear( DynamicString* input );

#endif
