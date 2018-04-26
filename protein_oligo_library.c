#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "protein_oligo_library.h"
#include "dynamic_string.h"

#define LINE_SIZE 256

void read_sequence( FILE* file_to_read, Sequence* in_sequence )
{
    int has_line;
    int index = 0;

    DynamicString* line = (DynamicString*) malloc( sizeof( DynamicString ) );
    DynamicString* sequence;

    has_line = get_a_line( file_to_read, line );
    while( has_line )
        {
            if( line->data[ 0 ] == '>' )
                {
                    sequence = (DynamicString*) malloc( sizeof( DynamicString ) );

                    in_sequence[ index ].name = line->data;
                    ds_init( sequence );
                    in_sequence[ index ].sequence = sequence; 
                    index++;
                }
            else
                {
                    ds_add( sequence, line->data );
                }
            ds_clear( line );
            has_line = get_a_line( file_to_read, line );
        }

    ds_clear( line );
    free( line );
}


int count_seqs_in_file( FILE* data_file )
{

    int counter = 0;
    char current_char = 0;

    if( !data_file )
        {
            return -1; 
        }
    while( current_char != EOF )
        {
            if( current_char == '>' )
                {
                    counter++;
                }
            current_char = fgetc( data_file );
        }
    fseek( data_file, 0, SEEK_SET );
    return counter;
}

int get_a_line( FILE* stream, DynamicString* to_read )
{
    char current_char[ 1 ] ;

    ds_init( to_read );

    do
        {

            current_char[ 0 ] = fgetc( stream );
            if( *current_char != '\n' )
                {
                    ds_add( to_read, current_char );
                }

        } while( *current_char != '\n' && *current_char != EOF );

    return !( *current_char == EOF );
}
