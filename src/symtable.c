/**
 * @file symtable.h
 * @brief Source file implementing a symbol table with hash-based storage for semantic analysis.
 * @authors Alex Marinica (xmarina00)
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include "symtable.h"
#include "stack.h"

#define INITIAL_CAPACITY 10 /**< Initial capacity of the symbol table. */
#define LOAD_FACTOR 0.75 /**< Maximum load factor before resizing the table. */

/**
 * @brief Hash function using the djb2 algorithm.
 * @param key The string key to hash.
 * @param capacity The current capacity of the hash table.
 * @return The computed hash value, modulo the table capacity.
 */
static unsigned int hash(const char *key, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;
}

/**
 * @brief Initializes an empty symbol table.
 * @return Pointer to the initialized symbol table.
 */
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

/**
 * @brief Frees all memory associated with a symbol table.
 * @param table Pointer to the symbol table to free.
 */
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

/**
 * @brief Resizes the symbol table and rehashes all existing symbols.
 * @param table Pointer to the symbol table to resize.
 */
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

/**
 * @brief Adds a function symbol to the symbol table.
 * @param table Pointer to the symbol table.
 * @param name The name of the function.
 * @param return_type The return type of the function.
 * @param is_initialized Whether the function is initialized.
 * @param fn_node Pointer to the function's AST node.
 * @param is_nullable Whether the function can return null.
 */
void add_function_symbol(SymbolTable *table, const char *name, DataType return_type, bool is_initialized, ASTNode *fn_node, bool is_nullable) {
    if ((float)table->count / table->capacity >= LOAD_FACTOR) {
        resize(table);
    }

    unsigned int index = hash(name, table->capacity);
    while (table->symbols[index] != NULL) {
        index = (index + 1) % table->capacity;
    }

    FuncSymbol *func = malloc(sizeof(FuncSymbol));
    if (!func) {
        exit(INTERNAL_ERROR);
    }

    func->name = strdup(name);
    func->type = return_type;
    func->has_return = false;
    func->used = false;
    func->is_nullable = is_nullable;
    func->is_initialized = is_initialized;
    func->fn_node = fn_node;
    func->scope_stack = init_scope_stack();

    Symbol *symbol = malloc(sizeof(Symbol));
    if (!symbol) {
        exit(INTERNAL_ERROR);
    }

    symbol->type = SYMBOL_FUNC;
    symbol->func = *func;

    table->symbols[index] = symbol;
    table->count++;
}

/**
 * @brief Adds a variable symbol to the symbol table.
 * @param table Pointer to the symbol table.
 * @param name The name of the variable.
 * @param type The data type of the variable.
 * @param is_constant Whether the variable is a constant.
 * @param is_nullable Whether the variable can hold null.
 * @param has_literal Whether the variable has a literal value.
 * @param value The initial value of the variable.
 */
void add_variable_symbol(SymbolTable *table, const char *name, DataType type, bool is_constant, bool is_nullable, bool has_literal, double value) {
    if ((float)table->count / table->capacity >= LOAD_FACTOR) {
        resize(table);
    }

    unsigned int index = hash(name, table->capacity);
    while (table->symbols[index] != NULL) {
        index = (index + 1) % table->capacity;
    }

    VarSymbol var = {.name = strdup(name), .type = type, .is_constant = false, .used = false, .is_nullable = false, .redefined = false, .has_literal = false, .value = 0};
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->type = SYMBOL_VAR;
    symbol->var = var;
    symbol->var.is_constant = is_constant;
    symbol->var.value = value;
    symbol->var.is_nullable = is_nullable;
    symbol->var.has_literal = has_literal;

    table->symbols[index] = symbol;
    table->count++;
}

/**
 * @brief Looks up a symbol by name in the symbol table.
 * @param table Pointer to the symbol table.
 * @param name The name of the symbol to look up.
 * @return Pointer to the found symbol, or NULL if not found.
 */
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
