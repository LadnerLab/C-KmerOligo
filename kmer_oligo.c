#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "protein_oligo_library.h"

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

    // parse options given from command lines
    while( ( option = getopt( argc, argv, "x:y:l:r:i:q:o:" ) ) != -1 )
        {
            switch( option )
                {
                case 'x':
                    xmer_window_size = *optarg - '0';
                    break;
                case 'y':
                    ymer_window_size = *optarg - '0';
                    break;
                case 'l':
                    min_length = *optarg - '0';
                    break;
                case 'r':
                    redundancy = *optarg - '0';
                    break;
                case 'i':
                    iterations = *optarg - '0';
                    break;
                case 'q':
                    query = optarg;
                    break;
                case 'o':
                    output = optarg;
                    break;
                }
        }

    FILE* data_file = fopen( query, "r" );
    int index;

    if( !data_file )
        {
            printf( "File not found.\n" );

            return EXIT_FAILURE;
        }

    int num_seqs = count_seqs_in_file( data_file );
    Sequence **sequences = malloc( sizeof( Sequence * ) * num_seqs );

    read_sequence( data_file, sequences );


    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( sequences[ index ]->sequence );
        }
    return EXIT_SUCCESS;
}


