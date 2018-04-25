#include <stdlib.h>
#include <stdio.h>
#include "protein_oligo_library.h"
#define DATA_FILE "test.fasta"



int main( void )
{

    Sequence* sequences = read_fasta_lists( DATA_FILE );

    return EXIT_SUCCESS;
}
