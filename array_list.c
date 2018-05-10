#define DEFAULT_CAPACITY 256

void ar_init( array_list_t* to_init )
{
    if( capacity > 0 )
        {
            to_init->data = malloc( sizeof( void* ) * DEFAULT_CAPACITY ); 
            to_init->size = 0;
            to_init->capacity = DEFAULT_CAPACITY;
        }
}


void ar_clear( array_list_t* to_clear )
{
    unsigned int index;
    for( index = 0; index < to_clear->size; index++ )
        {
            free( to_clear->array_data[ index ] );
        }
    free( to_clear );
}


void *ar_get( array_list_t* to_get, unsigned int index )
{
    if( index < to_get->size )
        {
            return to_get->array_data[ index ];
        }
    return NULL;
}
