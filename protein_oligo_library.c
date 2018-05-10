#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "protein_oligo_library.h"
#include "dynamic_string.h"
#include "array_list.h"

#define LINE_SIZE 256
#define HT_SURPLUS 1024
#define DASH_CHAR '-'

void read_sequences( FILE* file_to_read, sequence_t** in_sequence )
{
    int has_line;
    int index = 0;

    dynamic_string_t* line = (dynamic_string_t*) malloc( sizeof( dynamic_string_t ) );
    dynamic_string_t* sequence = malloc( sizeof( dynamic_string_t ) );

    has_line = get_a_line( file_to_read, line );
    while( has_line )
        {
            *( in_sequence + index ) = malloc( sizeof( sequence_t ) );

            if( line->data[ 0 ] == '>' )
                {
                    sequence = malloc( sizeof( dynamic_string_t ) );
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

void write_fastas( sequence_t** in_seqs, int num_seqs, char* output )
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
            current_char =  fgetc( data_file );
        }
    fseek( data_file, 0, SEEK_SET );
    return counter;
}

int get_a_line( FILE* stream, dynamic_string_t* to_read )
{
    char current_char[ 1 ] ;

    ds_init( to_read );

    while( *current_char != '\n' && (int) *current_char != EOF )
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


int calc_num_subseqs( int length, int window_size )
{
    return length - window_size + 1;
}

hash_table_t* create_xmers_with_locs( sequence_t* in_seq, int window_size, int step_size )
{
    int outer_index;
    int inner_index;
    int num_subsets = calc_num_subseqs( in_seq->sequence->size, window_size );

    hash_table_t *xmers_seq;
    
    subset_data_t* current_xmer_data;
    array_list_t* xmer_locations;

    char current_xmer[ window_size ];

    xmers_seq = malloc( sizeof( hash_table_t ) );
    ht_init( xmers_seq, num_subsets + HT_SURPLUS );

    for( outer_index = 0; outer_index < num_subsets; outer_index++ )
        {
            current_xmer_data = malloc( sizeof( subset_data_t ) );
            current_xmer_data->start = ( outer_index * step_size );

            for( inner_index = 0; inner_index < window_size; inner_index++ )
                {
                    current_xmer[ inner_index ] = in_seq->sequence->data[ ( outer_index * step_size ) + inner_index ];
                }

            current_xmer_data->end = ( outer_index * step_size ) + window_size;
            xmer_locations = ( array_list_t* ) ht_find( xmers_seq, current_xmer );

            if( xmer_locations != NULL )
                {
                    // update the entry at this location
                    ar_add( xmer_locations, current_xmer_data );
                }
            else
                {
                    // create the entry at this location
                    xmer_locations = malloc( sizeof( array_list_t ) );
                    ar_init( xmer_locations );
                    ar_add( xmer_locations, current_xmer_data );
 
                    ht_add( xmers_seq, current_xmer, xmer_locations );
                }
        }

    return xmers_seq;
}
