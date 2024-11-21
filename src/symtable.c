#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

#define INITIAL_CAPACITY 10
#define LOAD_FACTOR 0.75

/**
 * @brief Hash function using the djb2 algorithm to distribute keys across the symbol table.
 * 
 * @param key The string key to hash.
 * @return The generated hash value as an unsigned integer.
 */
unsigned int hash(const char *key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (unsigned int) hash;
}

/**
 * @brief Initializes an empty symbol table with open addressing.
 * 
 * @return A pointer to the newly created SymbolTable, or NULL if memory allocation fails.
 */
SymbolTable *init_symbol_table() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->symbols = malloc(INITIAL_CAPACITY * sizeof(Symbol *));
    for (int i = 0; i < INITIAL_CAPACITY; i++) {
        table->symbols[i] = NULL;  // Set all positions to NULL initially
    }
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

/**
 * @brief Resizes the symbol table to double its current capacity and rehashes existing symbols.
 * 
 * @param table Pointer to the SymbolTable to resize.
 */
void resize(SymbolTable *table) {
    int old_capacity = table->capacity;
    Symbol **old_symbols = table->symbols;
    
    // Double the capacity for the new table size
    table->capacity *= 2;
    table->symbols = malloc(table->capacity * sizeof(Symbol *));
    for (int i = 0; i < table->capacity; i++) {
        table->symbols[i] = NULL;
    }
    table->count = 0;

    // Rehash and insert symbols into the new table
    for (int i = 0; i < old_capacity; i++) {
        if (old_symbols[i] != NULL) {
            add_symbol(table, old_symbols[i]->name, old_symbols[i]->value);
            free(old_symbols[i]->name);
            free(old_symbols[i]);
        }
    }
    free(old_symbols);
}

/**
 * @brief Adds a symbol to the symbol table with linear probing for collision resolution.
 * 
 * @param table Pointer to the SymbolTable where the symbol will be added.
 * @param name Name of the symbol to add.
 * @param value Value of the symbol to store in the table.
 */
void add_symbol(SymbolTable *table, const char *name, int value) {
    if ((float)table->count / table->capacity >= LOAD_FACTOR) {
        resize(table); // Resize if load factor exceeds threshold
    }

    unsigned int index = hash(name) % table->capacity;
    // Linear probing to resolve collisions
    while (table->symbols[index] != NULL && strcmp(table->symbols[index]->name, name) != 0) {
        index = (index + 1) % table->capacity;
    }

    if (table->symbols[index] == NULL) {
        table->symbols[index] = malloc(sizeof(Symbol));
        table->symbols[index]->name = strdup(name);
        table->count++;
    }
    table->symbols[index]->value = value;
}

/**
 * @brief Looks up a symbol in the table using linear probing for collision handling.
 * 
 * @param table Pointer to the SymbolTable to search.
 * @param name Name of the symbol to look up.
 * @return Pointer to the found Symbol, or NULL if not found.
 */
Symbol *lookup_symbol(SymbolTable *table, const char *name) {
    unsigned int index = hash(name) % table->capacity;
    // Linear probing to locate the symbol
    while (table->symbols[index] != NULL) {
        if (strcmp(table->symbols[index]->name, name) == 0) {
            return table->symbols[index];
        }
        index = (index + 1) % table->capacity;
    }
    return NULL;  // Symbol not found
}

/**
 * @brief Frees memory allocated for the symbol table and its contents.
 * 
 * @param table Pointer to the SymbolTable to free.
 */
void free_symbol_table(SymbolTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        if (table->symbols[i] != NULL) {
            free(table->symbols[i]->name);
            free(table->symbols[i]);
        }
    }
    free(table->symbols);
    free(table);
}

#include <stdio.h>

/**
 * @brief Prints the contents of the symbol table for debugging.
 * 
 * @param table Pointer to the SymbolTable to print.
 */
void print_symbol_table(SymbolTable *table) {
    printf("Symbol Table:\n");
    for (int i = 0; i < table->capacity; i++) {
        if (table->symbols[i] != NULL) {
            printf("Index %d: Name=%s, Value=%d\n", i, table->symbols[i]->name, table->symbols[i]->value);
        } else {
            printf("Index %d: NULL\n", i);
        }
    }
}

