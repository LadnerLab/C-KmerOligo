#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "protein_oligo_library.h"
#include "hash_table.h"
#include "dynamic_string.h"

#define LINE_SIZE 256
#define DASH_CHAR '-'

void read_sequence( FILE* file_to_read, Sequence** in_sequence )
{
    int has_line;
    int index = 0;

    DynamicString* line = (DynamicString*) malloc( sizeof( DynamicString ) );
    DynamicString* sequence = malloc( sizeof( DynamicString ) );

    has_line = get_a_line( file_to_read, line );
    while( has_line )
        {
            *( in_sequence + index ) = malloc( sizeof( Sequence ) );

            if( line->data[ 0 ] == '>' )
                {
                    sequence = malloc( sizeof( DynamicString ) );
                    ds_init( sequence );

                    in_sequence[ index ]->name = line->data;
                    in_sequence[ index ]->sequence = sequence; 
                    index++;
                }
            else
                {
                    ds_add( sequence, line->data );
                }
            ds_init( line );
            has_line = get_a_line( file_to_read, line );
        }

    ds_clear( line );
}

void write_fastas( Sequence** in_seqs, int num_seqs, char* output )
{
    FILE* out_file = fopen( output, "w+" );
    int index;

    if( !out_file )
        {
            printf( "Unable to open file %s for output. Exiting..." , output );
            exit( EXIT_FAILURE );
        }

    for( index = 0; index < num_seqs; index++ )
        {
            fprintf( out_file, ">%s\n", in_seqs[ index ]->name );

            fprintf( out_file, "%s\n", in_seqs[ index ]->sequence->data );
        }

    fclose( out_file );
}


int count_seqs_in_file( FILE* data_file )
{

    int counter = 0;
    int current_char = 0;

    if( !data_file )
        {
            return -1; 
        }
    while( current_char != EOF )
        {
            if( (char) current_char == '>' )
                {
                    counter++;
                }
            current_char = (char) fgetc( data_file );
        }
    fseek( data_file, 0, SEEK_SET );
    return counter;
}

int get_a_line( FILE* stream, DynamicString* to_read )
{
    char current_char[ 1 ] ;

    ds_init( to_read );

        while( *current_char != '\n' && *current_char != EOF )
        {
            current_char[ 0 ] = (char) fgetc( stream );
            if( *current_char != '\n' ) 
                {
                    ds_add( to_read, current_char );
                }

        }
    return !( *current_char == EOF );
}


int count_char_in_string( char* string_in, char to_find )
{
    int count = 0;
    int index = 0;

    while( *(string_in + index ) )
        {
            if( *( string_in + index ) == to_find )
                {
                    count++;
                }
            index++;
        }

    return count;
}

int char_in_string( char* string_in, char to_find )
{
    int index = 0;
    while( *( string_in + index ) )
        {
            if ( *(string_in + index ) == to_find )
                {
                    return true;
                }
            index++;
        }
    return false;
}

float percent_char_in_string( char* string_in, char test_char )
{
    return ( (float) count_char_in_string( string_in, test_char ) /
             string_length( string_in ) ) * 100 ;
}

void remove_char_from_string( char* test_string, char to_remove )
{
    int index;
    int valid_index = 0;

    for( index = 0; test_string[ index ]; index++ )
        {
            if( test_string[ index ] == to_remove )
                {
                    continue;
                }

            test_string[ valid_index ] = test_string[ index ];

            valid_index++;

        }
    *( test_string + valid_index ) = '\0';
}

int is_valid_sequence( char* sequence, int min_length, float percent_valid )
{
    if( !char_in_string( sequence, 'X' ) )
        {
            if( !min_length )
                {
                    return percent_char_in_string( sequence, DASH_CHAR )
                           < ( 100 - percent_valid );
                }
            return count_char_in_string( sequence, DASH_CHAR )
                   <= string_length( sequence ) - min_length;
        }
   return 0;
}


int calc_num_subseqs( int length, int window_size, int step_size )
{
    return length - window_size + 1;
}

char** subset_lists( Sequence* in_seq, int window_size, int step_size )
{
    int outer_index;
    int inner_index;

    int num_subsets = calc_num_subseqs( in_seq->sequence->size, window_size, step_size );

    char** subset_seq = malloc( sizeof( char* ) * num_subsets + 1 );
    
    for( outer_index = 0; outer_index < num_subsets; outer_index++ )
        {
            subset_seq[ outer_index ] =  malloc( window_size * sizeof( char ) );
        }

    for( outer_index = 0; outer_index < num_subsets; outer_index++ )
        {
            for( inner_index = 0; inner_index < window_size; inner_index++ )
                {
                    subset_seq[ outer_index ][ inner_index ] = '\0';
                    subset_seq[ outer_index ][ inner_index ] = in_seq->sequence->data[ ( outer_index * step_size ) + inner_index ];
                }
        }

    subset_seq[ num_subsets ] = NULL;
    
    return subset_seq;
    
}
