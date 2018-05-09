#ifndef HASHTTABLE_HH_INCLUDED
#define HASHTTABLE_HH_INCLUDED

#define ITEM_NOT_FOUND -1

typedef struct HT_Entry
{
    char* key;
    void* value;
} HT_Entry;

typedef struct HashTable
{
    HT_Entry** table_data; 
    int capacity;
    int size;
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
 * @param input string to calculate hash for 
 * @return integer value representing hash of object
 **/
int generate_hash( char* key );

/**
 * Adds an entry to the hashtable
 * Note: Uses quadratic probing to resolve collisions
 * @param table pointer to HashTable to add data to
 * @param to_add string value to add as key to hash_table
 * @param add_val value to be added as value of the key
 * @returns integer value representing success of addition to table
 **/
int ht_add( HashTable* table, char* to_add, void* add_val );


/**
 * Finds the index of an item in the hash table
 * Note: Uses quadratic probing to resolve collisions
 * @param table pointer to HashTable whose capacity is used to 
 *        calculate initial index
 * @param in_key string key value to be searched for in the table
 * @returns integer index of the item, or ITEM_NOT_FOUND if the item
 *          is not in the table
 **/
int find_item_index( HashTable* table, char* in_key );

/**
 * Finds an item within a hash table using provided key.
 * Note: Uses find_item_index
 * @param table pointer to HashTable to search
 * @param in_key String key value to use to search
 * @returns pointeger to the integer found with the key in_key
 *          or NULL if the value was not found
 **/
void *ht_find( HashTable* table, char* in_key );


/**
 * Removes and frees data found in a hash table
 * Note: Uses find_item_index
 * @param table pointer to HashTable to delete from
 * @param in_key String key value to search
 * @returns integer true if item was successfully found and deleted
 *          integer false otherwise
 **/
int ht_delete( HashTable* table, char* in_key );
#endif

