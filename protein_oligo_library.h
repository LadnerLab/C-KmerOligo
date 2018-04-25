#ifndef PROTEIN_OLIGO_H_INCLUDED
#define PROTEIN_OLIGO_H_INCLUDED

typedef struct SEQUENCE
{
    char* name;
    char* sequence;
} Sequence;
int count_seqs_in_file( FILE* data_file );

#endif
