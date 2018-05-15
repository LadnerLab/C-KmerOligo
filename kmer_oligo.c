#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "protein_oligo_library.h"
#include "hash_table.h"
#include "array_list.h"

// program defaults
#define DEFAULT_XMER_SIZE 100
#define DEFAULT_YMER_SIZE 100
#define DEFAULT_REDUNDANCY 1
#define DEFAULT_STEP_SIZE 1
#define DEFAULT_PERCENT_VALID 90.00
#define DEFAULT_ITERATIONS 1
#define DEFAULT_OUTPUT "output.fasta"

#define YMER_TABLE_SIZE 100000


int main( int argc, char* argv[] )
{

    // option variables
    int xmer_window_size = DEFAULT_XMER_SIZE;
    int ymer_window_size = DEFAULT_YMER_SIZE;
    int min_length = 0;
    int redundancy = DEFAULT_REDUNDANCY;
    int iterations = DEFAULT_ITERATIONS;
    float percent_valid = DEFAULT_PERCENT_VALID;

    char* query = NULL;
    char* output = DEFAULT_OUTPUT;

    int option;

    // program variables
    FILE* data_file;
    sequence_t **seqs_from_file;

    hash_table_t* ymer_name_table;
    hash_table_t* ymer_table;
    hash_table_t* ymer_index_table;
    hash_table_t* xmer_table;

    HT_Entry** total_ymers;

    set_t* current_ymer_locs;

    int current_iteration;
    int num_seqs;
    int ymer_index;
    int max_score;
    int index;
    int inner_index;

    sequence_t* current_seq;

    char* current_ymer;
    char index_str[ DEFAULT_YMER_SIZE ];

    // parse options given from command lines
    while( ( option = getopt( argc, argv, "x:y:l:r:i:q:o:" ) ) != -1 )
        {
            switch( option )
                {
                case 'x':
                    xmer_window_size = atoi( optarg ); 
                    break;
                case 'y':
                    ymer_window_size = atoi( optarg );
                    break;
                case 'l':
                    min_length = atoi( optarg );
                    break;
                case 'r':
                    redundancy = atoi( optarg );
                    break;
                case 'i':
                    iterations = atoi( optarg );
                    break;
                case 'q':
                    query = optarg;
                    break;
                case 'o':
                    output = optarg;
                    break;
                }
        }

    data_file = fopen( query, "r" );
    if( !data_file )
        {
            printf( "Fasta query file either not found or not provided, exiting.\n" );

            return EXIT_FAILURE;
        }

    num_seqs = count_seqs_in_file( data_file );
    seqs_from_file = malloc( sizeof( sequence_t * ) * num_seqs );

    read_sequences( data_file, seqs_from_file );


    ymer_table = malloc( sizeof( hash_table_t ) );
    ymer_name_table = malloc( sizeof( hash_table_t ) );

    ymer_index_table = malloc( sizeof( hash_table_t ) );

    xmer_table = malloc( sizeof( hash_table_t ) );

    ht_init( ymer_name_table, YMER_TABLE_SIZE );
    ht_init( ymer_table, YMER_TABLE_SIZE );
    ht_init( xmer_table, YMER_TABLE_SIZE );
    ht_init( ymer_index_table, YMER_TABLE_SIZE );

    // create a table with xmers and their locations
    // create a table with ymers and their sequence names
    // create a table with ymers and their locations
    for( index = 0; index < num_seqs; index++ )
        {
            sprintf( index_str, "%d", index );
            current_seq = seqs_from_file[ index ];
            create_xmers_with_locs( xmer_table, index_str,
                                    current_seq->sequence->data,
                                    xmer_window_size, 1 );
        }
    for( index = 0; index < num_seqs; index++ )
        {
            sprintf( index_str, "%d", index );

            current_seq = seqs_from_file[ index ];
            create_xmers_with_locs( ymer_table, current_seq->name,
                                    current_seq->sequence->data,
                                    ymer_window_size, 1 );

            /* create_xmers_with_locs( ymer_index_table, index_str, */
            /*                         current_seq->sequence->data, */
            /*                         ymer_window_size, 1 ); */

            total_ymers = ht_get_items( ymer_table );
            printf( "%d\n", ymer_table->size );
            for( inner_index = 0; inner_index < ymer_table->size; inner_index++ )
                {
                    current_ymer = total_ymers[ inner_index ]->key;
                    if( is_valid_sequence( current_ymer, min_length, percent_valid ) )
                        {
                            current_ymer_locs = malloc( sizeof( set_t ) );
                            set_init( current_ymer_locs );

                            component_xmer_locs( current_ymer, total_ymers[ inner_index ]->key,
                                                                            current_ymer_locs, xmer_table, xmer_window_size, 1 );
                            ht_add( ymer_index_table, current_ymer, current_ymer_locs );
                        }
                }

            ht_clear( ymer_table );
            ymer_table = malloc( sizeof( hash_table_t ) );
            ht_init( ymer_table, YMER_TABLE_SIZE );
        }
    return EXIT_SUCCESS;


    for( ymer_index = 0; ymer_index < ymer_table->capacity; ymer_index++ )
        {
            set_t* current_ymer_locs = component_xmer_locs( "1", total_ymers[ ymer_index ]->key, current_ymer_locs, xmer_table, xmer_window_size, 1 );
        }

    printf( "ymer valid size: %d\n", ymer_table->size );

    current_iteration = 0;
    
    while( current_iteration < iterations )
        {
            max_score = 0;

            set_t* set = malloc( sizeof( set_t ) );
            set_init( set );
            component_xmer_locs( "1", total_ymers[ 0 ]->key, set, xmer_table, xmer_window_size, 1 );

            for( ymer_index = 0; ymer_index < ymer_table->capacity; ymer_index++ )
                {
                    if( ymer_table->table_data[ ymer_index ] != NULL )
                        {
                            array_list_t* current_data = (array_list_t*) ht_find( ymer_index_table, ymer_table->table_data[ ymer_index ]->key );
                            if( current_data->size < max_score )
                                {
                                    max_score = current_data->size;
                                }
                        }
                }


            current_iteration++;
        }
    // free all of our allocated memory
    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( seqs_from_file[ index ]->sequence );
        }

    free( seqs_from_file );

    free( total_ymers );
    ht_clear( ymer_name_table );
    ht_clear( ymer_table );


    return EXIT_SUCCESS;
}


