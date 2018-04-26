#include <stdlib.h>
#include <stdio.h>
#include "protein_oligo_library.h"
#define DATA_FILE "test.fast"



int main( int argc, char* argv[] )
{

    FILE* data_file = fopen( "test.fasta", "r" );

    if( !data_file )
        {
            printf( "File not found.\n" );

            return EXIT_FAILURE;
        }

    int num_seqs = count_seqs_in_file( data_file );
    printf( "%d\n", num_seqs );
    Sequence sequences[ num_seqs ];

    read_sequence( data_file, sequences );

    printf( "%s\n", sequences[ 0 ].sequence->data );
    printf( "%s\n", sequences[ 1 ].sequence->data );

    return EXIT_SUCCESS;
}
