#include <stdio.h>
#include <stdlib.h>

#include "array_list.h"

int main( void )
{
    array_list_t arr;
    
    ar_init( &arr );

    int one = 1;
    int two = 2;
    int three = 3;

    ar_add( &arr, &one );
    ar_add( &arr, &two );
    ar_add( &arr, &three );
    int data;
    unsigned int size = arr.size;
    for( unsigned int index = 0; index < size; index++ )
        {
            data = *( int* ) ar_remove( &arr, 0 );
            printf( "%d\n", data );
        }
            printf( "%d\n", arr.size );

}
