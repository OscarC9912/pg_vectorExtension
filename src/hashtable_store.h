#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100 // Hashtable size

// Define the structure for a hashtable entry
typedef struct Entry {
    char key[50];         // Key (e.g., table name)
    int value;            // Value
    struct Entry *next;   // Pointer to the next entry (for collisions)
} Entry;

// Define the structure for the hashtable
typedef struct Hashtable {
    Entry *buckets[TABLE_SIZE]; // Array of linked lists (for collisions)
} Hashtable;

// Hash function to calculate an index for a given key
unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++; // Simple hash function
    }
    return hash % TABLE_SIZE;
}

// Function to initialize a hashtable
Hashtable *create_hashtable() {
    Hashtable *table = malloc(sizeof(Hashtable));
    if (!table) {
        perror("Failed to create hashtable");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

// Function to insert a key-value pair into the hashtable
void table_insert(Hashtable *table, const char *key, int value) {
    unsigned int index = hash(key);
    Entry *new_entry = malloc(sizeof(Entry));
    if (!new_entry) {
        perror("Failed to allocate memory for entry");
        exit(EXIT_FAILURE);
    }
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = table->buckets[index]; // Insert at the head of the linked list
    table->buckets[index] = new_entry;
}

// Function to retrieve a value by key
int get_value(Hashtable *table, const char *key) {
    unsigned int index = hash(key);
    Entry *current = table->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value; // Return the value if the key matches
        }
        current = current->next; // Check the next entry in case of collision
    }
    return -1; // Return -1 if the key is not found
}

// // Main function to demonstrate usage
// int main() {
//     // Create the hashtable
//     Hashtable *table = create_hashtable();

//     // Insert key-value pairs
//     insert(table, "table1", 1000);
//     insert(table, "table2", 3000);

//     // Access values directly after declaration
//     printf("Value for 'table1': %d\n", get_value(table, "table1")); // Output: 1000
//     printf("Value for 'table2': %d\n", get_value(table, "table2")); // Output: 3000
//     printf("Value for 'table3': %d\n", get_value(table, "table3")); // Output: -1 (not found)

//     return 0;
// }