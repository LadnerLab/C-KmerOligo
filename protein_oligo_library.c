#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "protein_oligo_library.h"
#include "dynamic_string.h"

#define LINE_SIZE 256

Sequence *read_fasta_lists( char* file_to_read )
{
    FILE* data_file;
    int num_records;
    DynamicString* current_line;



    data_file = fopen( file_to_read, "r" );

    if( !data_file )
        {
            printf( "File not found: %s.", file_to_read );
            exit( EXIT_FAILURE );
        }

    num_records = count_seqs_in_file( data_file );
    Sequence* seqs_from_file = (Sequence*) malloc( sizeof( Sequence ) );

    for( int index = 0; index < num_records; index++ )
        {
            current_line = get_a_line( data_file );
        }


    ds_clear( current_line );
    free( current_line );
    fclose( data_file );

    return seqs_from_file;
}


int count_seqs_in_file( FILE* data_file )
{

    int counter = 0;
    char line[ 256 ];

    if( !data_file )
        {
            return -1; 
        }
    while( fscanf( data_file, "%s", line ) == 1 )
        {
            if( *( line ) == '>' )
                {
                    counter++;
                }
        }
    fseek( data_file, 0, SEEK_SET );
    return counter;
}

DynamicString* get_a_line( FILE* stream )
{
    char current_char[ 1 ] ;
    DynamicString* return_str = (DynamicString*) malloc( sizeof( DynamicString ) );

    ds_init( return_str );

    do
        {

            current_char[ 0 ] = fgetc( stream );
            ds_add( return_str, current_char );

        } while( *current_char != '\n' && *current_char != EOF );

    return return_str;
}
