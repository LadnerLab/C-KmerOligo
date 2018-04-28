#ifndef HASHTTABLE_HH_INCLUDED
#define HASHTTABLE_HH_INCLUDED

typedef struct HashTable
{
    HT_Entry* table_data; 
    int capacity;
} HashTable;

typedef struct HT_Entry
{
    char* key;
    void* value;
} HT_Entry;


/**
 * Initializes a HastTable struct. Allocates memory
 * to store size number of HT_Entry structs.
 * @param table pointer to HashTable to init
 * @param size integer size of HashTable
 **/
void ht_init( HashTable* table, int size );


#endif
