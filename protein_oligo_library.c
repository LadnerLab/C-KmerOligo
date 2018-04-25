#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "protein_oligo_library.h"

#define LINE_SIZE 256

Sequence* read_fasta_lists( char* file_to_read )
{
    FILE* data_file;
    int num_records;

    char* str_buffer = malloc( LINE_SIZE * sizeof( char ) );

    int current_char;
    int capacity = LINE_SIZE;
    int num_chars = 0;


    data_file = fopen( file_to_read, "r" );

    if( !data_file )
        {
            printf( "File not found: %s.", file_to_read );
            exit( EXIT_FAILURE );
        }

    num_records = count_seqs_in_file( data_file );

    Sequence seqs_from_file[ num_records ];

    current_char = fgetc( data_file );
    while( current_char != EOF )
        {
            if( num_chars >= capacity - 1 )
                {
                    capacity += LINE_SIZE;
                    str_buffer = realloc( str_buffer, capacity );
                }

            current_char = getc( data_file );

            *( str_buffer + num_chars ) = current_char;

            num_chars++;
        }

    str_buffer[ num_chars ] = '\0';

    printf( "%s\n", str_buffer );

    fclose( data_file );
    return NULL;
}



/**
 * Counts the amount of fastas in a fasta file provided
 * 
 * Note: Resets file pointer
 * @param file_to_read fasta file to be opened and read
 * @returns int number of sequences in file, or -1 if
 *          file could not be opened
**/
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
