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

/**
 * @brief Represents a function symbol in the symbol table.
 */
typedef struct FuncSymbol {
    char *name;              /**< Name of the function. */
    DataType type;           /**< Return type of the function. */
    bool has_return;         /**< Indicates if the function has a return statement if not of void type. */
    bool used;               /**< Indicates if the function has been called. */
    bool is_nullable;        /**< Indicates if the function's expression can be of `AST_NULL` value. */
    bool is_initialized;     /**< Indicates if the function has been initialized. */
    ASTNode *fn_node;        /**< Pointer to the AST node representing the function declaration. */
    ScopeStack *scope_stack; /**< Pointer to a ScopeStack for managing the function's scope. */
} FuncSymbol;

/**
 * @brief Represents a variable symbol in the symbol table.
 */
typedef struct VarSymbol {
    char *name;              /**< Name of the variable. */
    DataType type;           /**< Data type of the variable. */
    bool is_constant;        /**< Indicates if the variable is a constant. */
    bool used;               /**< Indicates if the variable has been used. */
    bool is_nullable;        /**< Indicates if the variable's value can be `AST_NULL`. */
    bool redefined;          /**< Indicates if the variable has been redefined. */
    bool has_literal;        /**< Indicates if the variable has a literal value. */
    float value;             /**< The value of the variable. */
} VarSymbol;

/**
 * @brief Enumerates the types of symbols: functions and variables.
 */
typedef enum {
    SYMBOL_FUNC, /**< Function symbol. */
    SYMBOL_VAR   /**< Variable symbol. */
} SymbolType;

/**
 * @brief Represents a symbol in the symbol table, which can be either a function or a variable.
 */
typedef struct Symbol {
    SymbolType type; /**< Type of symbol: `SYMBOL_FUNC` or `SYMBOL_VAR`. */
    union {
        FuncSymbol func; /**< Function symbol details. */
        VarSymbol var;   /**< Variable symbol details. */
    };
} Symbol;

/**
 * @brief Represents the symbol table.
 */
typedef struct SymbolTable {
    Symbol **symbols; /**< Array of pointers to symbols. */
    int count;        /**< Current number of symbols in the table. */
    int capacity;     /**< Maximum capacity of the table. */
} SymbolTable;

/**
 * @brief Initializes a new symbol table.
 *
 * Allocates memory for the symbol table and sets initial capacity and count.
 *
 * @return Pointer to the initialized symbol table.
 */
SymbolTable *init_symbol_table();

/**
 * @brief Frees the memory associated with a symbol table.
 *
 * Releases all memory allocated for the symbols and the table itself.
 *
 * @param table Pointer to the symbol table to free.
 */
void free_symbol_table(SymbolTable *table);

/**
 * @brief Resizes the symbol table when the load factor is exceeded.
 *
 * Doubles the capacity of the symbol table and rehashes all symbols.
 *
 * @param table Pointer to the symbol table to resize.
 */
void resize(SymbolTable *table);

/**
 * @brief Adds a function symbol to the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param name Name of the function.
 * @param return_type Return type of the function.
 * @param is_initialized Indicates if the function is initialized.
 * @param fn_node Pointer to the AST node for the function declaration.
 * @param is_nullable Indicates if the function can have a nullable return value.
 */
void add_function_symbol(SymbolTable *table, const char *name, DataType return_type, bool is_initialized, ASTNode *fn_node, bool is_nullable);

/**
 * @brief Adds a variable symbol to the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param name Name of the variable.
 * @param type Data type of the variable.
 * @param is_constant Indicates if the variable is constant.
 * @param is_nullable Indicates if the variable can have a nullable value.
 * @param has_literal Indicates if the variable has a literal value.
 * @param value Value of the variable.
 */
void add_variable_symbol(SymbolTable *table, const char *name, DataType type, bool is_constant, bool is_nullable, bool has_literal, double value);

/**
 * @brief Looks up a symbol in the symbol table by its name.
 *
 * @param table Pointer to the symbol table.
 * @param name Name of the symbol to look for.
 * @return Pointer to the symbol if found, or `NULL` if not found.
 */
Symbol *lookup_symbol(SymbolTable *table, const char *name);

#endif // SYMTABLE_H
