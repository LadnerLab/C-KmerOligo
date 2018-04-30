#ifndef HASHTTABLE_HH_INCLUDED
#define HASHTTABLE_HH_INCLUDED

typedef struct HT_Entry
{
    char* key;
    int value;
} HT_Entry;

typedef struct HashTable
{
    HT_Entry** table_data; 
    int capacity;
} HashTable;


/**
 * Initializes a HastTable struct. Allocates memory
 * to store size number of HT_Entry structs.
 * @param table pointer to HashTable to init
 * @param size integer size of HashTable
 **/
void ht_init( HashTable* table, int size );

/**
 * Clears a HashTable struct. Frees the memory
 * allocated for each HT_entry
 * @param table HashTable object to free
 **/
void ht_clear( HashTable* table );

/**
 * Generates the hash for use by the table.
 * @param input pointer to HT_Entry object with key to hash
 * @return integer value representing hash of object
 **/
int generate_hash( HT_Entry* input );

/**
 * Adds an entry to the hashtable
 * Note: Uses quadratic probing to resolve collisions
 * @param table pointer to HashTable to add data to
 * @param to_add string value to add as key to hash_table
 * @param add_val value to be added as value of the key
 **/
void ht_add( HashTable* table, char* to_add, int add_val );
#endif
