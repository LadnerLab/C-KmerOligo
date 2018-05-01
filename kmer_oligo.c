#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "protein_oligo_library.h"

#define DATA_FILE "test.fast"




int main( int argc, char* argv[] )
{

    // option variables
    int xmer_window_size = 0;
    int ymer_window_size = 0;
    int min_length = 0;
    int redundancy = 0;
    int iterations = 1;
    float percent_valid = 0;

    char* query = NULL;
    char* output = NULL;

    int option;

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



    FILE* data_file = fopen( "test.fasta", "r" );
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


