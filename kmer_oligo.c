#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>

#include "protein_oligo_library.h"
#include "hash_table.h"
#include "array_list.h"

// program defaults
#define DEFAULT_XMER_SIZE 100
#define DEFAULT_YMER_SIZE 100
#define DEFAULT_REDUNDANCY 1
#define DEFAULT_STEP_SIZE 1
#define DEFAULT_PERCENT_VALID 90.00
#define DEFAULT_ITERATIONS 1
#define DEFAULT_OUTPUT "output.fasta"

#define YMER_TABLE_SIZE 10000


int main( int argc, char* argv[] )
{

    // option variables
    int xmer_window_size = DEFAULT_XMER_SIZE;
    int ymer_window_size = DEFAULT_YMER_SIZE;
    int min_length = 0;
    int redundancy = DEFAULT_REDUNDANCY;
    int iterations = DEFAULT_ITERATIONS;
    float percent_valid = DEFAULT_PERCENT_VALID;

    char* query = NULL;
    char* output = DEFAULT_OUTPUT;

    int option;

    // program variables
    FILE* data_file;
    sequence_t **seqs_from_file;

    hash_table_t* ymer_name_table;
    hash_table_t* ymer_table;
    hash_table_t* ymer_index_table;
    hash_table_t* xmer_table;

    hash_table_t* array_design = NULL ;
    hash_table_t* current_ymer_xmers;
    hash_table_t* array_xmers;
    array_list_t* to_add;

    HT_Entry** total_ymers;

    set_t* current_ymer_locs;
    set_t* covered_locations;

    int current_iteration;
    int total_ymer_count = 0;
    uint32_t num_seqs;
    uint32_t ymer_index;
    uint64_t max_score;
    uint32_t index;
    uint32_t inner_index;

    sequence_t* current_seq;

    char* current_ymer;
    char* oligo_to_remove;
    char index_str[ DEFAULT_YMER_SIZE ];

    // parse options given from command lines
    while( ( option = getopt( argc, argv, "x:y:l:r:i:q:o:" ) ) != -1 )
        {
            switch( option )
                {
                case 'x':
                    xmer_window_size = atoi( optarg ); 
                    break;
                case 'y':
                    ymer_window_size = atoi( optarg );
                    break;
                case 'l':
                    min_length = atoi( optarg );
                    break;
                case 'r':
                    redundancy = atoi( optarg );
                    break;
                case 'i':
                    iterations = atoi( optarg );
                    break;
                case 'q':
                    query = optarg;
                    break;
                case 'o':
                    output = optarg;
                    break;
                }
        }

    data_file = fopen( query, "r" );
    if( !data_file )
        {
            printf( "Fasta query file either not found or not provided, exiting.\n" );

            return EXIT_FAILURE;
        }

    num_seqs = count_seqs_in_file( data_file );
    seqs_from_file = malloc( sizeof( sequence_t * ) * num_seqs );

    read_sequences( data_file, seqs_from_file );


    ymer_table = malloc( sizeof( hash_table_t ) );
    ymer_name_table = malloc( sizeof( hash_table_t ) );

    ymer_index_table = malloc( sizeof( hash_table_t ) );

    array_xmers = malloc( sizeof( hash_table_t ) );
    xmer_table = malloc( sizeof( hash_table_t ) );


    ht_init( ymer_name_table, YMER_TABLE_SIZE );
    ht_init( ymer_table, YMER_TABLE_SIZE );
    ht_init( xmer_table, YMER_TABLE_SIZE );
    ht_init( ymer_index_table, YMER_TABLE_SIZE );
    ht_init( array_xmers, YMER_TABLE_SIZE );

 for( index = 0; index < num_seqs; index++ )
        {
            sprintf( index_str, "%d", index );
            current_seq = seqs_from_file[ index ];
            create_xmers_with_locs( xmer_table, index_str,
                                    current_seq->sequence->data,
                                    xmer_window_size, 1 );
        }
    for( index = 0; index < num_seqs; index++ )
        {
            sprintf( index_str, "%d", index );

            current_seq = seqs_from_file[ index ];
            create_xmers_with_locs( ymer_table, current_seq->name,
                                    current_seq->sequence->data,
                                    ymer_window_size, 1
                                  );


            total_ymers = ht_get_items( ymer_table );
            for( inner_index = 0; inner_index < ymer_table->size; inner_index++ )
                {
                    current_ymer = total_ymers[ inner_index ]->key;

                    if( is_valid_sequence( current_ymer, min_length, percent_valid ) &&
                        ht_find( ymer_index_table, current_ymer ) == NULL )
                        {
                            current_ymer_locs = malloc( sizeof( set_t ) );
                            set_init( current_ymer_locs );

                            component_xmer_locs( current_ymer, total_ymers[ inner_index ]->key,
                                                 current_ymer_locs, xmer_table, xmer_window_size, 1
                                               );
                            ht_add( ymer_name_table, current_ymer, ht_find( ymer_table, current_ymer ) );

                            ht_add( ymer_index_table, current_ymer, current_ymer_locs );


                        }
                }

            // clear the table
            ht_clear( ymer_table );
            ymer_table = malloc( sizeof( hash_table_t ) );
            ht_init( ymer_table, YMER_TABLE_SIZE );
        }

    current_iteration = 0;
    
    to_add = malloc( sizeof( set_t ) );
    ar_init( to_add );

    total_ymer_count = ymer_index_table->size;

    set_t *current_data;
    while( current_iteration < iterations )
        {

            array_design = malloc( sizeof( hash_table_t ) );
            ht_init( array_design, YMER_TABLE_SIZE );
            do
                {
                    max_score = 0;

                    total_ymers = ht_get_items( ymer_index_table );
                    for( ymer_index = 0; ymer_index < ymer_index_table->size; ymer_index++ )
                        {
                            current_data = total_ymers[ ymer_index ]->value;
                            if( current_data->data->size > max_score )
                                {
                                    max_score = current_data->data->size;

                                    ar_clear( to_add );

                                    to_add = malloc( sizeof( set_t ) );

                                    ar_init( to_add );

                                    ar_add( to_add, total_ymers[ ymer_index ]->key );
                                }
                            else if( current_data->data->size == max_score )
                                {
                                    ar_add( to_add, total_ymers[ ymer_index ]->key );
                                }
                        }

                    oligo_to_remove = to_add->array_data[ rand() % to_add->size ];
                    covered_locations = ht_find( ymer_index_table, oligo_to_remove );

                    ht_add( array_design, oligo_to_remove, covered_locations );
                    ht_delete( ymer_index_table, oligo_to_remove );


                    current_ymer_xmers = malloc( sizeof( hash_table_t ) );
                    ht_init( current_ymer_xmers, YMER_TABLE_SIZE );


                    subset_lists( current_ymer_xmers, oligo_to_remove,
                                            xmer_window_size, 1
                                          );
                                            
                    HT_Entry** xmer_items = ht_get_items( current_ymer_xmers );
                    int xmer_value = 0;

                    for( index = 0; index < current_ymer_xmers->size; index++ )
                        {
                            if( ht_find( array_xmers,
                                         xmer_items[ index ]->key
                                       ) == NULL
                               )
                                {
                                    ht_add( array_xmers,
                                            xmer_items[ index ]->key,
                                            &xmer_value
                                          );
                                }
                            else
                                {
                                    ( *(int*) ht_find( array_xmers,
                                                     xmer_items[ index ]->key
                                                       )
                                    )++;
                                }
                        }

                    free( total_ymers );
                    ht_clear( current_ymer_xmers );

                    total_ymers = ht_get_items( ymer_index_table );
                    for( ymer_index = 0; ymer_index < ymer_index_table->size; ymer_index++ )
                        {
                                    current_data = total_ymers[ ymer_index ]->value;
                                    set_difference( current_data, covered_locations );

                        }
                    printf( "%d\n", ymer_index_table->size );

                } while( ymer_index_table->size > 0 &&
                         max_score > 0 &&
                         current_iteration < iterations
                       );

            current_iteration++;
        }


    printf( "Final design includes %d %d-mers ( %.1f%% of total ).\n", array_design->size,
            ymer_window_size, ( array_design->size / (float) total_ymer_count ) * 100
          );

    printf( "%d unique %d-mers in final %d-mers ( %.2f%% of total ).\n",
            array_xmers->size, xmer_window_size, ymer_window_size,
            ( (float) array_xmers->size / xmer_table->size ) * 100
            );
    // free all of our allocated memory
    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( seqs_from_file[ index ]->sequence );
        }

    free( seqs_from_file );

    free( total_ymers );
    ht_clear( ymer_name_table );
    ht_clear( ymer_table );


    return EXIT_SUCCESS;
}


