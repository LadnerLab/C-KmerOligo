#ifndef PROTEIN_OLIGO_H_INCLUDED
#define PROTEIN_OLIGO_H_INCLUDED
#include "dynamic_string.h"
#include "hash_table.h"

typedef struct Sequence
{
    char* name;
    DynamicString* sequence;
} Sequence;

typedef struct subset_data_t
{
    unsigned int start;
    unsigned int end;
} subset_data_t;

/**
 * Reads all of the fastas in given file
 * @param file_to_read string name of file to read
 * @return array of pointers to sequences containing sequence data
 *         found in file
 **/
void read_sequences( FILE* file_to_read, Sequence** in_sequence );

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

/**
 * Counts the number of specified character in a string
 * @param string_in pointer to first character of a string
 * @param to_find character to count in the string
 * @return integer number of to_find character found in string
 **/
int count_char_in_string( char* string_in, char to_find );


/**
 * Tests for character in a string
 * @param string_in pointer to first character in a string
 * @param to_find character value to test for
 * @returns boolean result of test
 **/
int char_in_string( char* string_in, char to_find );

/**
 * Calculates the percentage of specified charater makes up string
 * @param string_in string to be tested
 * @param test_char character to test for in string
 * @return floating point percentage of character in string
 **/
float percent_char_in_string( char* string_in, char test_char );

/**
 * Writes fastas to an output file, from an array of Sequences.
 * @param in_seqs array of pointers to sequences to write
 * @param num_seqs the number of sequences to write to the file
 * @param output string filename to write to
 **/
void write_fastas( Sequence** in_seqs, int num_seqs, char* output );

/**
 * Removes a character from a string in place
 * Note: String passed into function must be declared as a character array,
 * not a character pointer.
 * @param test_string String from which to remove the character
 * @param to_remove character to remove from string 
 **/
void remove_char_from_string( char* test_string, char to_remove );

/**
 * Determines whether a given sequence is or is not valid
 * @param sequence String sequence to be tested
 * @param minimum length of non dash characters needed to be present
          in order for the string to count
 * @param percent_valid Percentage of characters needed to be non-dash in order
          for the string to count
**/
int is_valid_sequence( char* sequence, int min_length, float percent_valid );
/**
 * Creates a hashtable of all the window_size-mers from a sequence
 * @param in_seq pointer to input sequence to create a subset of 
 * @param window_size integer number of characters to capture with each iteration
 * @param step_size integer number of characters to move over after each iteration
 * @returns pointer to hash table containing all of the subsets of the sequence, 
 *          as key, and subset_data_t as key containing start/end
 **/ 
HashTable* subset_lists( Sequence* in_seq, int window_size, int step_size );

#endif
