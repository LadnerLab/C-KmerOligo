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
    hash_table_t* ymer_table;
    char** subset;

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
    ht_init( ymer_table, 10000 );

    hash_table_t* ymer_tableau = malloc( sizeof( hash_table_t ) );
    ht_init( ymer_tableau, 100000 );

    int inner_index = 0;
    for( index = 0; index < num_seqs; index++ )
        {
            ymer_table = create_xmers_with_locs( seqs_from_file[ index ], ymer_window_size, 1 );


    for( inner_index = 0; index < ymer_table->capacity; index++ )
        {
            if( ymer_table->table_data[ index ] )
                {
                    printf( "%s\n", ymer_table->table_data[ index ]->key);
                    ht_add( ymer_tableau, ymer_table->table_data[ index ]->key, ymer_table->table_data[ index ]->value );
                }
        }

    }

    
    printf( "%d\n", ymer_tableau->size );


    // free all of our allocated memory
    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( seqs_from_file[ index ]->sequence );
        }

    free( seqs_from_file );
    /* ht_clear( ymer_table ); */
    return EXIT_SUCCESS;
}


