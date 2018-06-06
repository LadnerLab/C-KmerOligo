#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

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

int sum_values_of_table( hash_table_t* in_table );
void write_outputs( hash_table_t* output_oligos, hash_table_t* name_table,
                    char* outfile_name, int redundancy );

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
    HT_Entry** xmer_items;
    HT_Entry** total_ymers_clear;

    set_t* current_ymer_locs;
    set_t* covered_locations;

    int current_iteration;
    int *xmer_value = NULL;
    int total_ymer_count = 0;
    uint32_t num_seqs;
    uint32_t ymer_index;
    uint64_t max_score;
    uint32_t index;
    uint32_t inner_index;
    uint32_t min_ymers;

    set_t* current_set;
    set_t *current_data;

    sequence_t* current_seq;
    array_list_t* tracked_data;

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

    tracked_data = malloc( sizeof( array_list_t* ) );
    ar_init( tracked_data );

    num_seqs = count_seqs_in_file( data_file );
    seqs_from_file = malloc_track( tracked_data, sizeof( sequence_t * ) * num_seqs );


    read_sequences( data_file, seqs_from_file );

    ymer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );
    ymer_name_table = malloc_track( tracked_data, sizeof( hash_table_t ) );

    ymer_index_table = malloc_track( tracked_data, sizeof( hash_table_t ) );

    array_xmers = malloc_track( tracked_data, sizeof( hash_table_t ) );
    xmer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );


    ht_init( ymer_name_table, YMER_TABLE_SIZE );
    ht_init( ymer_table, YMER_TABLE_SIZE );
    ht_init( xmer_table, YMER_TABLE_SIZE );
    ht_init( ymer_index_table, YMER_TABLE_SIZE );
    ht_init( array_xmers, YMER_TABLE_SIZE );

    // seed our random number
    srand( time( NULL ) );

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

            ht_clear( ymer_table );
            free( total_ymers );
 
            ymer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );
            ht_init( ymer_table, YMER_TABLE_SIZE );
        }

    current_iteration = 0;

    total_ymer_count = ymer_index_table->size;
    array_design = malloc_track( tracked_data, sizeof( hash_table_t ) );
    ht_init( array_design, YMER_TABLE_SIZE );

    min_ymers = ymer_index_table->size;
 
    while( current_iteration < iterations )
        {

           do
                {
                    to_add = malloc( sizeof( set_t ) );
                    ar_init( to_add );
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
                    ht_init( current_ymer_xmers, calc_num_subseqs( ymer_window_size, xmer_window_size ) );


                    subset_lists( current_ymer_xmers, oligo_to_remove,
                                            xmer_window_size, 1
                                          );
                                            
                    xmer_items = ht_get_items( current_ymer_xmers );

                    for( index = 0; index < current_ymer_xmers->size; index++ )
                        {
                            xmer_value = malloc( 1 );
                            *xmer_value = 1;

                            if( ht_find( array_xmers,
                                         xmer_items[ index ]->key
                                       ) == NULL
                               )
                                {
                                    ht_add( array_xmers,
                                            xmer_items[ index ]->key,
                                            xmer_value
                                          );
                                }
                            else
                                {
                                    free( xmer_value );
                                    ( *(int*) ht_find( array_xmers,
                                                       xmer_items[ index ]->key
                                                     )
                                    )++;
                                }
                        }



                    free( total_ymers );
                    free( xmer_items );

                    total_ymers = ht_get_items( ymer_index_table );
                    for( ymer_index = 0; ymer_index < ymer_index_table->size; ymer_index++ )
                        {
                                    current_data = total_ymers[ ymer_index ]->value;
                                    set_difference( current_data, covered_locations );

                        }
                    printf( "%d\n", ymer_index_table->size );

                    free( total_ymers );
                    ht_clear( current_ymer_xmers );
                    ar_clear( to_add );
                    free( current_ymer_xmers );

                } while( ymer_index_table->size > 0 &&
                         max_score > 0 &&
                         current_iteration < iterations
                       );


           if( array_design->size < min_ymers )
               {
                   min_ymers = array_design->size;
               }

           total_ymers_clear = ht_get_items( ymer_index_table );
           for( index = 0; index < ymer_index_table->size; index++ )
               {
                   current_set = total_ymers_clear[ index ]->value;
                   set_clear( current_set );
               }

           // statistics output
           printf( "Final design includes %d %d-mers ( %.1f%% of total ).\n", array_design->size,
                   ymer_window_size, ( array_design->size / (float) total_ymer_count ) * 100
                   );

           printf( "%d unique %d-mers in final %d-mers ( %.2f%% of total ).\n",
                   array_xmers->size, xmer_window_size, ymer_window_size,
                   ( (float) array_xmers->size / xmer_table->size ) * 100 
                   );

           printf( "Average redundancy of %d-mers in %d-mers: %.2f\n",
                   xmer_window_size, ymer_window_size,
                   ( (float) sum_values_of_table( array_xmers ) / xmer_table->size ) );

           current_iteration++;

        }


              
    // write output to specified file
    write_outputs( array_design, ymer_name_table, output, redundancy );

    
    // free all of our allocated memory
    ht_clear( ymer_name_table );
    ht_clear( xmer_table );
    ht_clear( ymer_index_table );
    ht_clear( array_xmers );
    ht_clear( array_design );
    for( index = 0; index < num_seqs; index++ )
        {
            ds_clear( seqs_from_file[ index ]->sequence );
        }

    free_data( tracked_data );
    ar_clear( tracked_data );
    return EXIT_SUCCESS;
}


int sum_values_of_table( hash_table_t* in_table )
{
    uint32_t index;
    int total = 0;
    
    HT_Entry** table_values = ht_get_items( in_table );

    for( index = 0; index < in_table->size; index++ )
        {
            total += *( (int*) table_values[ index ]->value );
        }

    free( table_values );
    return total;
}

void write_outputs( hash_table_t* output_oligos, hash_table_t* name_table,
                    char* outfile_name, int redundancy
                  )
{
    uint32_t index;
    uint32_t num_ymers = output_oligos->size;
    
    HT_Entry** array_design_items = ht_get_items( output_oligos );
    HT_Entry* current_item = NULL;

    sequence_t* output_seqs[ num_ymers ];
    sequence_t* to_write;

    dynamic_string_t *ymer;

    int outfile_len = strlen( outfile_name );
    // padding for characters added to string
    int extra_chars = 12;

    char* ymer_name = NULL;
    char outfile_name_with_redundancy[ outfile_len + extra_chars ];
    
    for( index = 0; index < num_ymers; index++ )
        {
            to_write = malloc( sizeof( sequence_t ) );
            ymer = malloc( sizeof( dynamic_string_t ) );

            current_item = find_item( name_table, array_design_items[ index ]->key );

            ymer_name = (char*) ( ( *(array_list_t*)current_item->value ).array_data[ 0 ] );

            ymer->data = current_item->key;

            to_write->name = ymer_name;
            to_write->sequence = ymer;
            
            output_seqs[ index ] = to_write;
        }

    sprintf( outfile_name_with_redundancy, "%s_R_%d", outfile_name, redundancy ); 
    write_fastas( output_seqs, num_ymers, outfile_name_with_redundancy );

    for( index = 0; index < num_ymers; index++ )
        {
            free( output_seqs[ index ]->sequence );
            free( output_seqs[ index ] );
        }

    free( array_design_items );
}
