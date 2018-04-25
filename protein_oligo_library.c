#include <stdio.h>
#include <stdlib.h>

char* read_fasta_lists( char* file_to_read )
{
    return file_to_read;
}



/**
 * Counts the amount of fastas in a fasta file provided
 * 
 * @param file_to_read fasta file to be opened and read
 * @returns int number of sequences in file, or -1 if
 *          file could not be opened
**/
int count_seqs_in_file( char* data_file )
{
    FILE* open_file;

    int counter = 0;
    char line[ 100 ];

    open_file =  fopen( data_file, "r" );

    if( !open_file )
        {
            return -1; 
        }

    while( fscanf( open_file, "%s", line ) == 1 )
        {
            if( *( line ) == '>' )
                {
                    counter++;
                }
        }

    fclose( open_file );
    return counter;
}
