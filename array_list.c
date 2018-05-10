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
