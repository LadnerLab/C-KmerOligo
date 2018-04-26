#ifndef PROTEIN_OLIGO_H_INCLUDED
#define PROTEIN_OLIGO_H_INCLUDED
#include "dynamic_string.h"
typedef struct Sequence
{
    char* name;
    DynamicString* sequence;
} Sequence;

/**
 * Reads all of the fastas in given file
 * @param file_to_read string name of file to read
 * @return array of pointers to sequences containing sequence data
 *         found in file
 **/
void read_sequence( FILE* file_to_read, Sequence* in_sequence );

/**
 * Counts the amount of fastas in a fasta file provided
 * 
 * Note: Resets file pointer
 * @param file_to_read fasta file to be opened and read
 * @returns int number of sequences in file, or -1 if
 *          file could not be opened
**/
int count_seqs_in_file( FILE* data_file );

/**
 * Reads a line from a file into a new DynamicString object
 * @param stream open file pointer to read from
 * @returns DynamicString representation of the line
 **/
int get_a_line( FILE* stream, DynamicString* to_read );

#endif
