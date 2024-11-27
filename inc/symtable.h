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
    bool has_return;         /**< Indicates that a function has a return statement if not of void-type */
    bool used;               /**< Indicates if the function was called */
    bool is_nullable;        /**< Indicates if the function's expression can be of AST_NULL value */
    ScopeStack *scope_stack; /**< Pointer to a ScopeStack for function scope */
} FuncSymbol;

typedef struct VarSymbol {
    char *name;              /**< Name of the variable */
    DataType type;           /**< Data type of the variable */
    bool is_constant;        /**< Indicates if the variable is constant */
    bool used;               /**< Indicates if the variable was used */
    bool is_nullable;        /**< Indicates if the var's expression can be of AST_NULL value */
    bool redefined;          /**< Indicates if the variable was redefined */
    float value;             /**< Indicates the value of variable */
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
void add_variable_symbol(SymbolTable *table, const char *name, DataType type, bool is_constant, double value);
Symbol *lookup_symbol(SymbolTable *table, const char *name);
void print_symbol_table(const SymbolTable *table);

#endif // SYMTABLE_H
