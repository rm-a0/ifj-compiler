/**
 * @file symtable.h
 * @brief Source file implementing symbol table
 * @authors Alex Marinica (xmarina00)
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "symtable.h"
#include "stack.h"

#define INITIAL_CAPACITY 10
#define LOAD_FACTOR 0.75

/* Hash function using djb2 algorithm
   source: https://www.cse.yorku.ca/~oz/hash.html */
static unsigned int hash(const char *key, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;
}

/* Initialize an empty symbol table */
SymbolTable *init_symbol_table() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->symbols = malloc(INITIAL_CAPACITY * sizeof(Symbol *));
    for (int i = 0; i < INITIAL_CAPACITY; i++) {
        table->symbols[i] = NULL;
    }
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

/* Free a symbol table */
void free_symbol_table(SymbolTable *table) {
    if (table == NULL) {
        return;
    }
    
    if (table->symbols != NULL) {
        for (int i = 0; i < table->capacity; i++) {
            if (table->symbols[i] != NULL) {
                if (table->symbols[i]->type == SYMBOL_FUNC) {
                    free(table->symbols[i]->func.name);
                    free_scope_stack(table->symbols[i]->func.scope_stack); 
                } else if (table->symbols[i]->type == SYMBOL_VAR) {
                    free(table->symbols[i]->var.name);
                }
                free(table->symbols[i]);
            }
        }
        free(table->symbols);
    }
    
    free(table);
}


/* Resize the symbol table and rehash symbols */
void resize(SymbolTable *table) {
    int old_capacity = table->capacity;
    Symbol **old_symbols = table->symbols;

    table->capacity *= 2;
    table->symbols = malloc(table->capacity * sizeof(Symbol *));
    for (int i = 0; i < table->capacity; i++) {
        table->symbols[i] = NULL;
    }
    table->count = 0;

    for (int i = 0; i < old_capacity; i++) {
        if (old_symbols[i] != NULL) {
            Symbol *symbol = old_symbols[i];
            unsigned int index = hash(
                symbol->type == SYMBOL_FUNC ? symbol->func.name : symbol->var.name, 
                table->capacity
            );
            while (table->symbols[index] != NULL) {
                index = (index + 1) % table->capacity;
            }
            table->symbols[index] = symbol;
            table->count++;
        }
    }
    free(old_symbols);
}

/* Add a function symbol to the table */
void add_function_symbol(SymbolTable *table, const char *name, DataType return_type) {
    // Resize the table if the load factor threshold is reached
    if ((float)table->count / table->capacity >= LOAD_FACTOR) {
        resize(table);
    }

    unsigned int index = hash(name, table->capacity);
    // Find an empty slot using linear probing
    while (table->symbols[index] != NULL) {
        index = (index + 1) % table->capacity;
    }

    // Initialize the function symbol
    FuncSymbol func = {.name = strdup(name), .type = return_type, .has_return = false, .used = false, .is_nullable = false, .scope_stack = init_scope_stack()};
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->type = SYMBOL_FUNC;
    symbol->func = func;

    // Add the symbol to the table
    table->symbols[index] = symbol;
    table->count++;
}


/* Add a variable symbol to the table */
void add_variable_symbol(SymbolTable *table, const char *name, DataType type, bool is_constant, double value) {
    // Resize the table if the load factor threshold is reached
    if ((float)table->count / table->capacity >= LOAD_FACTOR) {
        resize(table);
    }

    unsigned int index = hash(name, table->capacity);
    // Find an empty slot using linear probing
    while (table->symbols[index] != NULL) {
        index = (index + 1) % table->capacity;
    }

    // Initialize the variable symbol
    VarSymbol var = {.name = strdup(name), .type = type, .is_constant = false, .used = false, .is_nullable = false, .redefined = false, .value = 0};
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->type = SYMBOL_VAR;
    symbol->var = var;
    symbol->var.is_constant = is_constant;
    symbol->var.value = value;

    // Add the symbol to the table
    table->symbols[index] = symbol;
    table->count++;
}


/* Lookup a symbol by name */
Symbol *lookup_symbol(SymbolTable *table, const char *name) {
    unsigned int index = hash(name, table->capacity);
    while (table->symbols[index] != NULL) {
        const char *current_name = table->symbols[index]->type == SYMBOL_FUNC
                                       ? table->symbols[index]->func.name
                                       : table->symbols[index]->var.name;
        if (strcmp(current_name, name) == 0) {
            return table->symbols[index];
        }
        index = (index + 1) % table->capacity;
    }
    return NULL;
}

/* Print the contents of the symbol table */
void print_symbol_table(const SymbolTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        if (table->symbols[i] != NULL) {
            Symbol *symbol = table->symbols[i];
            if (symbol->type == SYMBOL_FUNC) {
            } else if (symbol->type == SYMBOL_VAR) {
            }
        } else {
        }
    }
}
