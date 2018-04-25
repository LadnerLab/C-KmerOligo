#include <stdlib.h>
#include <stdio.h>
#include "protein_oligo_library.h"
#define DATA_FILE "test.fasta"



int main( void )
{

    int num_seqs = count_seqs_in_file( "test.fasta" );

    if( num_seqs == -1 )
        {
            printf( "Data file not found: %s.\n", DATA_FILE );

            return EXIT_FAILURE;
        }

    printf( "%d\n", num_seqs );

    return EXIT_SUCCESS;
}
