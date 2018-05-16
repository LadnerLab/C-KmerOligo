#include <stdio.h>
#include <stdlib.h>

#include "set.h"

void display_set( set_t* to_display );

int main( void )
{
    set_t* a = malloc( sizeof( set_t ) );
    set_t* b = malloc( sizeof( set_t ) );
    set_t* c = malloc( sizeof( set_t ) );
    set_init( a );
    set_init( b );
    set_init( c );
    char* del = "A";
    char* del2 = "B";
    char* del3 = "C";
    char* del4 = "D";
    char* del5 = "E";

    char **string = malloc( sizeof( char* ) * 3 );
    string[ 0 ] = "ABC";
    string[ 1 ] = "CDE";
    string[ 2 ] = "DFJ";


    int storage[ 25 ];

    int index;

    set_add( a, del );
    set_add( a, del2 );
    set_add( a, del3 );
    set_add( a, del4 );

    set_add( b, del2);
    set_add( b, del3 );
    set_add( b, del4 );
    set_add( b, del5 );

    set_difference( c, b, a );

    set_add_all( a, string, 3 );
    display_set( a );
    display_set( b );
    display_set( c );


    return EXIT_SUCCESS;

    
}

void display_set( set_t* to_display )
{
    unsigned int index;
    HT_Entry* found_item;


    for( index = 0; index < to_display->data->capacity; index++ )
        {
            found_item = to_display->data->table_data[ index ];
            if( found_item != NULL )
                {
                    printf( "%s ", to_display->data->table_data[ index ]->key );
                    while( found_item->next != NULL )
                        {
                            found_item = found_item->next;
                            printf( "%s ", found_item->key );
                        }
                }
            else
                {
                    printf( "N " );
                }
        }
    printf( "\n" );
}
