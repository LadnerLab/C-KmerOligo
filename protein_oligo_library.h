#ifndef PROTEIN_OLIGO_H_INCLUDED
#define PROTEIN_OLIGO_H_INCLUDED

typedef struct Sequence
{
    char* name;
    char* sequence;
} Sequence;

Sequence* read_fasta_lists( char* file_to_read );
int count_seqs_in_file( FILE* data_file );

#endif
