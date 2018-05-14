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

#define YMER_TABLE_SIZE 1000000


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
    int index;
    int num_seqs;
    hash_table_t* ymer_name_table;
    hash_table_t* ymer_valid_table;
    hash_table_t* ymer_index_table;

    hash_table_t* xmer_table;

    HT_Entry** total_ymers;

    sequence_t* current_seq;
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


    ymer_name_table = malloc( sizeof( hash_table_t ) );
    ymer_valid_table = malloc( sizeof( hash_table_t ) );

    ymer_index_table = malloc( sizeof( hash_table_t ) );

    xmer_table = malloc( sizeof( hash_table_t ) );

    ht_init( ymer_name_table, YMER_TABLE_SIZE );
    ht_init( xmer_table, YMER_TABLE_SIZE );
    ht_init( ymer_index_table, YMER_TABLE_SIZE );

    for( index = 0; index < num_seqs; index++ )
        {
            current_seq = seqs_from_file[ index ];
            create_xmers_with_locs( ymer_name_table, current_seq->name,
                                    current_seq->sequence->data,
                                    ymer_window_size, 1 );

            sprintf( index_str, "%d", index );
            create_xmers_with_locs( ymer_index_table, index_str,
                                    current_seq->sequence->data,
                                    ymer_window_size, 1 );

            create_xmers_with_locs( xmer_table, current_seq->name,
                                    current_seq->sequence->data,
                                    xmer_window_size, 1 );
        }

    ht_init( ymer_valid_table, ymer_name_table->size );


    total_ymers = ht_get_items( ymer_name_table );

    for( index = 0; index < ymer_name_table->size; index++ )
        {
            if( is_valid_sequence( total_ymers[ index ]->key, min_length, percent_valid ) )
                {
                    ht_add( ymer_valid_table, total_ymers[ index ]->key ,
                            total_ymers[ index ]->value );
                }
        }

    printf( "%d\n", xmer_table->size );
    printf( "Ymer name size: %d\n", ymer_name_table->size );
    printf( "ymer valid size: %d\n", ymer_valid_table->size );
    // free all of our allocated memory
    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( seqs_from_file[ index ]->sequence );
        }

    free( seqs_from_file );

    free( total_ymers );
    ht_clear( ymer_name_table );
    ht_clear( ymer_valid_table );


    return EXIT_SUCCESS;
}


