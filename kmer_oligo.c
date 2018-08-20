#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


#include "protein_oligo_library.h"
#include "hash_table.h"
#include "array_list.h"
#include "thpool.h"

#define ARGS "b:x:y:r:i:q:o:t:p::c:n:"

// program defaults
#define DEFAULT_XMER_SIZE 100
#define DEFAULT_YMER_SIZE 100
#define DEFAULT_REDUNDANCY 1
#define DEFAULT_STEP_SIZE 1
#define DEFAULT_PERCENT_VALID 90.00
#define DEFAULT_ITERATIONS 1
#define DEFAULT_OUTPUT "output.fasta"
#define DISPLAY_INTERVAL 100
#define DEFAULT_THREADS 1
#define DEFAULT_XMER_COVERAGE 1.0
#define BLOSUM_90 "blosum90"
#define BLOSUM_62 "blosum62"
#define DEFAULT_MIN_YMERS 0 

#define YMER_TABLE_SIZE 100000



// ================== PROTOTYPES ========================== 
void start_threads( int num_threads, int size,
                    set_t* covered_locs, HT_Entry* total_ymers,
                    threadpool thpool
                  );
void clear_blosum( blosum_data_t* to_clear );

void show_usage( char* program_name );

void thread_difference( void* arg );

int sum_values_of_table( hash_table_t* in_table );
FILE* blosum90();
FILE* blosum62();

void update_xmer_table_values( hash_table_t* current_ymer_xmers,
                               hash_table_t* xmer_table,
                               hash_table_t* array_xmers
                             );

void write_outputs( hash_table_t* output_oligos, hash_table_t* name_table,
                    char* outfile_name, int redundancy
                  );

typedef struct thread_info_t
{
    uint32_t start_index;
    uint32_t end_index;

    void (*difference_func)( set_t*, set_t* ); 
    set_t* covered_locations;
    HT_Entry* total_ymers;
} thread_info_t;

// ==========================================================

int main( int argc, char* argv[] )
{

    // option variables
    int xmer_window_size = DEFAULT_XMER_SIZE;
    int ymer_window_size = DEFAULT_YMER_SIZE;
    int redundancy = DEFAULT_REDUNDANCY;
    int iterations = DEFAULT_ITERATIONS;
    float percent_valid = DEFAULT_PERCENT_VALID;
    float min_xmer_coverage = DEFAULT_XMER_COVERAGE;

    char* query = NULL;
    char* output = DEFAULT_OUTPUT;

    int option;
    threadpool thpool;

    // program variables
    FILE* data_file;
    sequence_t **seqs_from_file;

    hash_table_t* ymer_name_table;
    hash_table_t* ymer_table;
    hash_table_t* ymer_index_table;
    hash_table_t* xmer_table;
    hash_table_t* best_iteration = NULL;

    hash_table_t* array_design = NULL ;
    hash_table_t* current_ymer_xmers;
    hash_table_t* array_xmers;
    array_list_t* to_add;

    blosum_data_t* blosum_data = NULL;

    HT_Entry* total_ymers;
    HT_Entry* xmer_items;
    HT_Entry* total_ymers_clear;

    set_t* current_ymer_locs;
    set_t* covered_locations;

    int current_iteration;
    int total_ymer_count = 0;
    int count_val = 0;
    int permute = 0;
    int blosum_cutoff = 0;
        
    uint32_t num_threads = DEFAULT_THREADS;
    uint32_t num_seqs;
    uint32_t ymer_index;
    uint64_t max_score;
    uint32_t index;
    uint32_t inner_index;
    uint32_t min_ymers = DEFAULT_MIN_YMERS;

    set_t *current_data;

    sequence_t* current_seq;
    array_list_t* tracked_data;

    char* current_ymer;
    char* blosum = NULL;
    char* oligo_to_remove;
    char index_str[ DEFAULT_YMER_SIZE ];

    // parse options given from command lines
    while( ( option = getopt( argc, argv, ARGS ) ) != -1 )
        {
            switch( option )
                {
                case 'x':
                    xmer_window_size = atoi( optarg ); 
                    break;
                case 'y':
                    ymer_window_size = atoi( optarg );
                    break;
                case 'p':
                    permute = 1;
                    break;
                case 'r':
                    redundancy = atoi( optarg );
                    break;
                case 'i':
                    iterations = atoi( optarg );
                    break;
                case 'c':
                    min_xmer_coverage = atof( optarg );
                    break;
                case 'q':
                    query = optarg;
                    break;
                case 'b':
                    blosum = optarg;
                    break;
                case 'n':
                    blosum_cutoff = atoi( optarg );
                    break;
                case 'o':
                    output = optarg;
                    break;
                case 't':
                    num_threads = atoi( optarg );
                    break;
                default:
                    show_usage( argv[ 0 ] );

                    return EXIT_SUCCESS;
                }
        }

    data_file = fopen( query, "r" );
    if( !data_file )
        {
            printf( "Fasta query file either not found or not provided, exiting.\n" );

            return EXIT_FAILURE;
        }

    if( blosum )
        {
            FILE* blosum_file = NULL;
            if( !strcmp( BLOSUM_90, blosum ) )
                {
                    blosum_file = blosum90();
                }
            else if( !strcmp( BLOSUM_62, blosum ) )
                {
                    blosum_file = blosum62();
                }
            else
                {
                    blosum_file = fopen( blosum, "r" );
                }
           blosum_data =  parse_blosum_file( blosum_file );

           fclose( blosum_file );
        }

    tracked_data = malloc( sizeof( array_list_t ) );
    ar_init( tracked_data );


    num_seqs = count_seqs_in_file( data_file );

    seqs_from_file = malloc( sizeof( sequence_t * ) * num_seqs );
    read_sequences( data_file, seqs_from_file );

    current_iteration = 0;
    while( current_iteration < iterations )
        {
            count_val = 0;

            ymer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );
            ymer_name_table = malloc_track( tracked_data, sizeof( hash_table_t ) );

            ymer_index_table = malloc_track( tracked_data, sizeof( hash_table_t ) );

            array_xmers = malloc_track( tracked_data, sizeof( hash_table_t ) );
            xmer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );

            array_design = malloc_track( tracked_data, sizeof( hash_table_t ) );

            ht_init( ymer_name_table, YMER_TABLE_SIZE );
            ht_init( ymer_table, YMER_TABLE_SIZE );
            ht_init( xmer_table, YMER_TABLE_SIZE );
            ht_init( ymer_index_table, YMER_TABLE_SIZE );
            ht_init( array_xmers, YMER_TABLE_SIZE );
            ht_init( array_design, YMER_TABLE_SIZE );

            if( num_threads > num_seqs )
                {
                    num_threads = num_seqs;
                }

            thpool = thpool_init( num_threads );

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
                            current_ymer = total_ymers[ inner_index ].key;

                            if( is_valid_sequence( current_ymer, 0, percent_valid ) &&
                                ht_find( ymer_index_table, current_ymer ) == NULL )
                                {
                                    current_ymer_locs = malloc( sizeof( set_t ) );
                                    set_init( current_ymer_locs );

                                    component_xmer_locs( current_ymer, total_ymers[ inner_index ].key,
                                                         current_ymer_locs, xmer_table, xmer_window_size, 1,
                                                         blosum_data,
                                                         blosum_cutoff,
                                                         permute
                                                         );
                                    ht_add( ymer_name_table, current_ymer, ht_find( ymer_table, current_ymer ) );

                                    ht_add( ymer_index_table, current_ymer, current_ymer_locs );

                                }
                            else
                                {
                                    ar_clear_and_free( total_ymers[ inner_index ].value );
                                }
                        }

                    ht_clear( ymer_table );
                    free( total_ymers );
 
                    ymer_table = malloc_track( tracked_data, sizeof( hash_table_t ) );
                    ht_init( ymer_table, YMER_TABLE_SIZE );
                }


            ht_clear( ymer_table );
            total_ymer_count = ymer_index_table->size;

            if( min_ymers == DEFAULT_MIN_YMERS )
                {
                    min_ymers = ymer_index_table->size;
                }
 
            do
                {
                    to_add = malloc( sizeof( array_list_t ) );
                    ar_init( to_add );
                    max_score = 0;

                    total_ymers = ht_get_items( ymer_index_table );
                    for( ymer_index = 0; ymer_index < ymer_index_table->size; ymer_index++ )
                        {
                            current_data = total_ymers[ ymer_index ].value;
                            if( current_data->data->size > max_score )
                                {
                                    max_score = current_data->data->size;

                                    ar_clear( to_add );

                                    to_add = malloc( sizeof( array_list_t ) );

                                    ar_init( to_add );

                                    ar_add( to_add, total_ymers[ ymer_index ].key );
                                }
                            else if( current_data->data->size == max_score )
                                {
                                    ar_add( to_add, total_ymers[ ymer_index ].key );
                                }
                        }

                    oligo_to_remove = to_add->array_data[ rand() % to_add->size ];
                    covered_locations = ht_find( ymer_index_table, oligo_to_remove );

                    ht_add( array_design, oligo_to_remove, NULL );
                    ht_delete( ymer_index_table, oligo_to_remove );

                    count_val++;
                    if( !( count_val % DISPLAY_INTERVAL ) )
                        {
                            printf( "Current iteration: %d.\n", count_val );
                            printf( "Current max score: %lu.\n", max_score );
                        }

                                                                     
                    current_ymer_xmers = malloc( sizeof( hash_table_t ) );
                    ht_init( current_ymer_xmers, calc_num_subseqs( ymer_window_size, xmer_window_size ) );


                    subset_lists( current_ymer_xmers, oligo_to_remove,
                                  xmer_window_size, 1,
                                  blosum_data,
                                  blosum_cutoff,
                                  permute
                                  );
                                            
                    update_xmer_table_values( current_ymer_xmers, xmer_table, array_xmers );

                    free( total_ymers );

                    total_ymers = ht_get_items( ymer_index_table );

                    start_threads( num_threads, ymer_index_table->size, covered_locations, total_ymers, thpool );

                    set_clear( covered_locations );
                    free( total_ymers );
                    ht_clear( current_ymer_xmers );
                    ar_clear( to_add );
                    free( current_ymer_xmers );

                } while( ymer_index_table->size > 0 &&
                         max_score > 0 &&
                         (float) array_xmers->size / xmer_table->size < min_xmer_coverage
                         );
            // statistics output
            printf( "\nFinal design includes %d %d-mers ( %.1f%% of total ).\n", array_design->size,
                    ymer_window_size, ( array_design->size / (float) total_ymer_count ) * 100
                    );

            printf( "%d unique %d-mers in final %d-mers ( %.2f%% of total ).\n",
                    array_xmers->size, xmer_window_size, ymer_window_size,
                    ( (float) array_xmers->size / xmer_table->size ) * 100 
                    );

            printf( "Average redundancy of %d-mers in %d-mers: %.2f\n",
                    xmer_window_size, ymer_window_size,
                    ( (float) sum_values_of_table( array_xmers ) / xmer_table->size ) );


            total_ymers = ht_get_items( ymer_index_table );
            for( index = 0; index < ymer_index_table->size; index++ )
                {
                    current_data = total_ymers[ index ].value;
                    total_ymers_clear = ht_get_items( current_data->data );
                    for( inner_index = 0; inner_index < current_data->data->size; inner_index++ )
                        {
                            free( total_ymers_clear[ inner_index ].key );
                        }
                    free( total_ymers_clear );
                }

            free( total_ymers );

            if( array_design->size < min_ymers )
                {
                    min_ymers = array_design->size;
                    best_iteration = array_design;

                    array_design = malloc_track( tracked_data, sizeof( hash_table_t ) );
                    ht_init( array_design, YMER_TABLE_SIZE );

                   // write output to specified file
                   write_outputs( best_iteration, ymer_name_table, output, redundancy );
                   ht_clear( best_iteration );

                }
            else
                {
                    ht_clear( array_design );
                    free( array_design );

                    array_design = NULL;
                }

            // free our allocated memory
            xmer_items = ht_get_items( xmer_table );
            for( index = 0; index < xmer_table->size; index++ ) {
                    ar_clear( xmer_items[ index ].value );
                }
            HT_Entry* all_ymers = ht_get_items( ymer_name_table );
            for( index = 0; index < ymer_name_table->size; index++ )
                {
                    for( inner_index = 0; inner_index <
                             ((array_list_t*) all_ymers[ index ].value )->size; inner_index++
                       )
                        {
                            free( ((array_list_t*) (all_ymers[ index ].value))->array_data[ inner_index ] );
                        }
                    ar_clear( all_ymers[ index ].value );
                }

            free( all_ymers );

            ht_clear( ymer_name_table );
            ht_clear( xmer_table );
            ht_clear( ymer_index_table );
            ht_clear( array_xmers );

            free( xmer_items );
 
            current_iteration++;
        }




   for( index = 0; index < num_seqs; index++ )
       {
           ds_clear( seqs_from_file[ index ]->sequence );
           free( seqs_from_file[ index ]->name );
           free( seqs_from_file[ index ] );
       }
   free( seqs_from_file );


   if( array_design )
       {
           ht_clear( array_design );
           free( array_design );
       }

    free_data( tracked_data );
    ar_clear( tracked_data );

    fclose( data_file );

    if( blosum_data )
        {
            clear_blosum( blosum_data );
        }

    return EXIT_SUCCESS;
}


int sum_values_of_table( hash_table_t* in_table )
{
    uint32_t index;
    int total = 0;
    
    HT_Entry* table_values = ht_get_items( in_table );

    for( index = 0; index < in_table->size; index++ )
        {
            total += *( (int*) table_values[ index ].value );
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
    
    HT_Entry* array_design_items = ht_get_items( output_oligos );
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

            current_item = find_item( name_table, array_design_items[ index ].key );

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

void thread_difference( void* arg )
{
    thread_info_t* info = arg;

    uint32_t index;
    uint32_t start_index = info->start_index;

    set_t* current_data;
    set_t* covered_locs = info->covered_locations;


    for( index = start_index; index < info->end_index; index++ )
        {
            current_data = info->total_ymers[ index ].value;

            info->difference_func( current_data, covered_locs );
        }
}


void start_threads( int num_threads, int size, set_t* covered_locs, HT_Entry* total_ymers, threadpool thpool )
{
    int seqs_per_thread = size / num_threads;
    int index;

    uint32_t start_index;
    uint32_t end_index;
    int remainder = size % num_threads;

    thread_info_t thread_info[ num_threads ];

    start_index = 0;

    for( index = 0; index < num_threads; index++ )
        {
            end_index = seqs_per_thread * ( index + 1 ) + remainder;

            thread_info[ index ].start_index = start_index;
            thread_info[ index ].end_index = end_index;
            thread_info[ index ].difference_func = &set_difference;
            thread_info[ index ].covered_locations = covered_locs;
            thread_info[ index ].total_ymers = total_ymers;

            thpool_add_work( thpool, thread_difference, &thread_info[ index ] );
            start_index = end_index;

            remainder = 0;
            
        }

    thpool_wait( thpool );


}

void show_usage( char* program_name )
{
    printf( "Usage: %s [ options ]\n ", program_name );
    puts( "-h, --help                 display this help and exit." );
    puts( " -x                         integer xmer window size. [None, Required]\n" );
    puts( " -y                         integer ymer window size. [None, Required]\n" );
    puts( " -r                         default redundancy to be used.[1]\n" );
    puts( " -i                         number of iterations to do. [1]\n" );
    puts( " -q                         fasta query file to perform operations on [None, Required]. \n" );
    puts( " -o                         name of file to output to [output.fasta]\n" );
    puts( " -t                         number of threads to use [1]\n" );
    puts( " -p                         include this flag in order to perform permutation of xmer functional groups\n" );
    puts( " -c                         floating point minimum xmer coverage [1]\n" );
    puts( " -b                         blosum matrix to be used in inclusion of xmer functional groups.\n"
          "                            Note that blosum90 and blosum62 are hard-coded into this program,\n"
          "                            and are specified by blosum90 or blosum62. Otherwise, specify a \n"
          "                            text file containing a blosum matrix.\n"
        );
    puts( " -n                         integer cutoff for whether an amino acid can be substituted \n"
          "                            only relationships greater to or equal to this number will be added [0] \n"
        );

}




FILE* blosum90()
{
    FILE* temp = tmpfile();
    fputs( "   A  R  N  D  C  Q  E  G  H  I  L  K  M  F  P  S  T  W  Y  V  B  J  Z  X  *   \n", temp );
    fputs( "A  5 -2 -2 -3 -1 -1 -1  0 -2 -2 -2 -1 -2 -3 -1  1  0 -4 -3 -1 -2 -2 -1 -1 -6\n", temp );   
    fputs( "R -2  6 -1 -3 -5  1 -1 -3  0 -4 -3  2 -2 -4 -3 -1 -2 -4 -3 -3 -2 -3  0 -1 -6\n", temp );
    fputs( "N -2 -1  7  1 -4  0 -1 -1  0 -4 -4  0 -3 -4 -3  0  0 -5 -3 -4  5 -4 -1 -1 -6\n", temp );
    fputs( "D -3 -3  1  7 -5 -1  1 -2 -2 -5 -5 -1 -4 -5 -3 -1 -2 -6 -4 -5  5 -5  1 -1 -6\n", temp );
    fputs( "C -1 -5 -4 -5  9 -4 -6 -4 -5 -2 -2 -4 -2 -3 -4 -2 -2 -4 -4 -2 -4 -2 -5 -1 -6\n", temp );
    fputs( "Q -1  1  0 -1 -4  7  2 -3  1 -4 -3  1  0 -4 -2 -1 -1 -3 -3 -3 -1 -3  5 -1 -6\n", temp );
    fputs( "E -1 -1 -1  1 -6  2  6 -3 -1 -4 -4  0 -3 -5 -2 -1 -1 -5 -4 -3  1 -4  5 -1 -6\n", temp );
    fputs( "G  0 -3 -1 -2 -4 -3 -3  6 -3 -5 -5 -2 -4 -5 -3 -1 -3 -4 -5 -5 -2 -5 -3 -1 -6\n", temp );
    fputs( "H -2  0  0 -2 -5  1 -1 -3  8 -4 -4 -1 -3 -2 -3 -2 -2 -3  1 -4 -1 -4  0 -1 -6\n", temp );
    fputs( "I -2 -4 -4 -5 -2 -4 -4 -5 -4  5  1 -4  1 -1 -4 -3 -1 -4 -2  3 -5  3 -4 -1 -6\n", temp );
    fputs( "L -2 -3 -4 -5 -2 -3 -4 -5 -4  1  5 -3  2  0 -4 -3 -2 -3 -2  0 -5  4 -4 -1 -6\n", temp );
    fputs( "K -1  2  0 -1 -4  1  0 -2 -1 -4 -3  6 -2 -4 -2 -1 -1 -5 -3 -3 -1 -3  1 -1 -6\n", temp );
    fputs( "M -2 -2 -3 -4 -2  0 -3 -4 -3  1  2 -2  7 -1 -3 -2 -1 -2 -2  0 -4  2 -2 -1 -6\n", temp );
    fputs( "F -3 -4 -4 -5 -3 -4 -5 -5 -2 -1  0 -4 -1  7 -4 -3 -3  0  3 -2 -4  0 -4 -1 -6\n", temp );
    fputs( "P -1 -3 -3 -3 -4 -2 -2 -3 -3 -4 -4 -2 -3 -4  8 -2 -2 -5 -4 -3 -3 -4 -2 -1 -6\n", temp );
    fputs( "S  1 -1  0 -1 -2 -1 -1 -1 -2 -3 -3 -1 -2 -3 -2  5  1 -4 -3 -2  0 -3 -1 -1 -6\n", temp );
    fputs( "T  0 -2  0 -2 -2 -1 -1 -3 -2 -1 -2 -1 -1 -3 -2  1  6 -4 -2 -1 -1 -2 -1 -1 -6\n", temp );
    fputs( "W -4 -4 -5 -6 -4 -3 -5 -4 -3 -4 -3 -5 -2  0 -5 -4 -4 11  2 -3 -6 -3 -4 -1 -6\n", temp );
    fputs( "Y -3 -3 -3 -4 -4 -3 -4 -5  1 -2 -2 -3 -2  3 -4 -3 -2  2  8 -3 -4 -2 -3 -1 -6\n", temp );
    fputs( "V -1 -3 -4 -5 -2 -3 -3 -5 -4  3  0 -3  0 -2 -3 -2 -1 -3 -3  5 -4  1 -3 -1 -6\n", temp );
    fputs( "B -2 -2  5  5 -4 -1  1 -2 -1 -5 -5 -1 -4 -4 -3  0 -1 -6 -4 -4  5 -5  0 -1 -6\n", temp );
    fputs( "J -2 -3 -4 -5 -2 -3 -4 -5 -4  3  4 -3  2  0 -4 -3 -2 -3 -2  1 -5  4 -4 -1 -6\n", temp );
    fputs( "Z -1  0 -1  1 -5  5  5 -3  0 -4 -4  1 -2 -4 -2 -1 -1 -4 -3 -3  0 -4  5 -1 -6\n", temp );
    fputs( "X -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -6\n", temp );
    fputs( "* -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6 -6  1\n", temp );
    ;

    fflush( temp );
    rewind( temp );

    return temp;
}

FILE* blosum62()
{
    FILE* temp = tmpfile();

    fputs( "   A  R  N  D  C  Q  E  G  H  I  L  K  M  F  P  S  T  W  Y  V  B  Z  X  *  \n", temp );
    fputs( "A  4 -1 -2 -2  0 -1 -1  0 -2 -1 -1 -1 -1 -2 -1  1  0 -3 -2  0 -2 -1  0 -4 \n", temp );
    fputs( "R -1  5  0 -2 -3  1  0 -2  0 -3 -2  2 -1 -3 -2 -1 -1 -3 -2 -3 -1  0 -1 -4 \n", temp );
    fputs( "N -2  0  6  1 -3  0  0  0  1 -3 -3  0 -2 -3 -2  1  0 -4 -2 -3  3  0 -1 -4 \n", temp );
    fputs( "D -2 -2  1  6 -3  0  2 -1 -1 -3 -4 -1 -3 -3 -1  0 -1 -4 -3 -3  4  1 -1 -4 \n", temp );
    fputs( "C  0 -3 -3 -3  9 -3 -4 -3 -3 -1 -1 -3 -1 -2 -3 -1 -1 -2 -2 -1 -3 -3 -2 -4 \n", temp );
    fputs( "Q -1  1  0  0 -3  5  2 -2  0 -3 -2  1  0 -3 -1  0 -1 -2 -1 -2  0  3 -1 -4 \n", temp );
    fputs( "E -1  0  0  2 -4  2  5 -2  0 -3 -3  1 -2 -3 -1  0 -1 -3 -2 -2  1  4 -1 -4 \n", temp );
    fputs( "G  0 -2  0 -1 -3 -2 -2  6 -2 -4 -4 -2 -3 -3 -2  0 -2 -2 -3 -3 -1 -2 -1 -4 \n", temp );
    fputs( "H -2  0  1 -1 -3  0  0 -2  8 -3 -3 -1 -2 -1 -2 -1 -2 -2  2 -3  0  0 -1 -4 \n", temp );
    fputs( "I -1 -3 -3 -3 -1 -3 -3 -4 -3  4  2 -3  1  0 -3 -2 -1 -3 -1  3 -3 -3 -1 -4 \n", temp );
    fputs( "L -1 -2 -3 -4 -1 -2 -3 -4 -3  2  4 -2  2  0 -3 -2 -1 -2 -1  1 -4 -3 -1 -4 \n", temp );
    fputs( "K -1  2  0 -1 -3  1  1 -2 -1 -3 -2  5 -1 -3 -1  0 -1 -3 -2 -2  0  1 -1 -4 \n", temp );
    fputs( "M -1 -1 -2 -3 -1  0 -2 -3 -2  1  2 -1  5  0 -2 -1 -1 -1 -1  1 -3 -1 -1 -4 \n", temp );
    fputs( "F -2 -3 -3 -3 -2 -3 -3 -3 -1  0  0 -3  0  6 -4 -2 -2  1  3 -1 -3 -3 -1 -4 \n", temp );
    fputs( "P -1 -2 -2 -1 -3 -1 -1 -2 -2 -3 -3 -1 -2 -4  7 -1 -1 -4 -3 -2 -2 -1 -2 -4 \n", temp );
    fputs( "S  1 -1  1  0 -1  0  0  0 -1 -2 -2  0 -1 -2 -1  4  1 -3 -2 -2  0  0  0 -4 \n", temp );
    fputs( "T  0 -1  0 -1 -1 -1 -1 -2 -2 -1 -1 -1 -1 -2 -1  1  5 -2 -2  0 -1 -1  0 -4 \n", temp );
    fputs( "W -3 -3 -4 -4 -2 -2 -3 -2 -2 -3 -2 -3 -1  1 -4 -3 -2 11  2 -3 -4 -3 -2 -4 \n", temp );
    fputs( "Y -2 -2 -2 -3 -2 -1 -2 -3  2 -1 -1 -2 -1  3 -3 -2 -2  2  7 -1 -3 -2 -1 -4 \n", temp );
    fputs( "V  0 -3 -3 -3 -1 -2 -2 -3 -3  3  1 -2  1 -1 -2 -2  0 -3 -1  4 -3 -2 -1 -4 \n", temp );
    fputs( "B -2 -1  3  4 -3  0  1 -1  0 -3 -4  0 -3 -3 -2  0 -1 -4 -3 -3  4  1 -1 -4 \n", temp );
    fputs( "Z -1  0  0  1 -3  3  4 -2  0 -3 -3  1 -1 -3 -1  0 -1 -3 -2 -2  1  4 -1 -4 \n", temp );
    fputs( "X  0 -1 -1 -1 -2 -1 -1 -1 -1 -1 -1 -1 -1 -1 -2  0  0 -2 -1 -1 -1 -1 -1 -4 \n", temp );
    



    fflush( temp );
    rewind( temp );

    return temp;
}

void clear_blosum( blosum_data_t* to_clear )
{
    uint32_t index;
    HT_Entry* blosum_table_data = ht_get_items( to_clear->blosum_table );

    for( index = 0; index < to_clear->blosum_table->size; index++ )
        {
            free( blosum_table_data[ index ].value );
        }

    ht_clear( to_clear->blosum_table );

    free( to_clear->blosum_table  );
    free( to_clear->letter_data );
    free( to_clear );
    free( blosum_table_data );
    
}

void update_xmer_table_values( hash_table_t* current_ymer_xmers, hash_table_t* xmer_table, hash_table_t* array_xmers )
{
    uint32_t index = 0;
    int *xmer_value;

    HT_Entry* xmer_items = NULL;


    xmer_items = ht_get_items( current_ymer_xmers );
    for( index = 0; index < current_ymer_xmers->size; index++ )
        {
            if( ht_find( xmer_table, xmer_items[ index ].key ) )
                {
                    xmer_value = malloc( sizeof( int ) );
                    *xmer_value = 1;

                    if( ht_find( array_xmers,
                                 xmer_items[ index ].key
                                 )
                        == NULL
                        )
                        {
                            ht_add( array_xmers,
                                    xmer_items[ index ].key,
                                    xmer_value
                                    );
                        }
                    else
                        {
                            free( xmer_value );
                            ( *(int*) ht_find( array_xmers,
                                               xmer_items[ index ].key
                                               )
                              )++;
                        }
                }
        }

    free( xmer_items );
}
