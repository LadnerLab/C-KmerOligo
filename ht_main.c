#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"

int main( void )
{
    hash_table_t* ht = malloc( sizeof( hash_table_t ) );

    int *art = malloc( sizeof( int ) );
    int park = 4;

    art = &park;
    

    ht_init( ht, 20 );


    ht_add( ht, "ABCD", art );
    ht_add( ht, "OBCJDK", art );
    ht_add( ht, "OBCADK", art );
    ht_add( ht, "OBOJDK", art );
    ht_add( ht, "ODCJDK", art );
    ht_add( ht, "OBCBDK", art );
    ht_add( ht, "OBCLDK", art );

    printf( "%d\n", *(int*)ht_find( ht, "ABCD" ) );
    free( ht_delete( ht, "ABCD" ) );
    /* printf( "%d\n", *(int*)ht_find( ht, "OBCJDK" ) ); */
    ht_clear( ht );


    return EXIT_SUCCESS;
}
