/**
 * @file symtable.h
 * @brief Header file for implementation of symbol table
 * @authors Alex Marinica (xmarina00)
*/

#ifndef SYMTABLE_H
#define SYMTABLE_H
#include "ast.h"
struct ScopeStack;
typedef struct ScopeStack ScopeStack;

typedef struct FuncSymbol {
    char *name;              /**< Name of the function */
    DataType type;           /**< Return type of function */
    bool used;               /**< Indicates if the function was called */
    ScopeStack *scope_stack; /**< Pointer to a ScopeStack for function scope */
} FuncSymbol;

typedef struct VarSymbol {
    char *name;              /**< Name of the variable */
    DataType type;           /**< Data type of the variable */
    bool used;               /**< Indicates if the variable was used */
    bool redefined;          /**< Indicates if the variable was redefined */
} VarSymbol;

typedef enum {
    SYMBOL_FUNC, /**< Function symbol */
    SYMBOL_VAR   /**< Variable symbol */
} SymbolType;

typedef struct Symbol {
    SymbolType type; /**< Type of symbol: FUNCTION or VARIABLE */
    union {
        FuncSymbol func;
        VarSymbol var;
    };
} Symbol;

typedef struct SymbolTable {
    Symbol **symbols; /**< Array of pointers to symbols */
    int count;        /**< Current number of symbols in the table */
    int capacity;     /**< Maximum capacity of the table */
} SymbolTable;

/* Function Prototypes */
SymbolTable *init_symbol_table();
void free_symbol_table(SymbolTable *table);
void resize(SymbolTable *table);
void add_function_symbol(SymbolTable *table, const char *name, DataType return_type);
void add_variable_symbol(SymbolTable *table, const char *name, DataType type);
Symbol *lookup_symbol(SymbolTable *table, const char *name);
void print_symbol_table(const SymbolTable *table);

#endif // SYMTABLE_H
