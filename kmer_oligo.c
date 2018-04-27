#include <stdlib.h>
#include <stdio.h>
#include "protein_oligo_library.h"
#define DATA_FILE "test.fast"



int main( int argc, char* argv[] )
{

    FILE* data_file = fopen( "test.fasta", "r" );
    int index;

    if( !data_file )
        {
            printf( "File not found.\n" );

            return EXIT_FAILURE;
        }

    int num_seqs = count_seqs_in_file( data_file );
    Sequence sequences[ num_seqs ];

    read_sequence( data_file, sequences );


    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( sequences[ index ].sequence );
        }
    return EXIT_SUCCESS;
}


