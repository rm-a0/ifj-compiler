#ifndef SYMTABLE_H
#define SYMTABLE_H

/**
 * @struct Symbol
 * @brief Struct representing a symbol in the symbol table.
 */
typedef struct Symbol {
    char *name; /**< Name of the symbol */
    int value;  /**< Value of the symbol, for demonstration purposes */
} Symbol;

/**
 * @struct SymbolTable
 * @brief Struct representing a symbol table with open addressing for collision handling.
 */
typedef struct SymbolTable {
    Symbol **symbols; /**< Array of pointers to Symbol entries */
    int count;        /**< Number of symbols in the table */
    int capacity;     /**< Capacity of the symbol table */
} SymbolTable;

// Symbol table management prototypes

/**
 * @brief Initializes an empty symbol table for a frame.
 * @return Pointer to the initialized SymbolTable.
 */
SymbolTable *init_symbol_table();

/**
 * @brief Adds a symbol to the symbol table.
 * @param table Pointer to the SymbolTable.
 * @param name Name of the symbol to add.
 * @param value Value of the symbol.
 */
void add_symbol(SymbolTable *table, const char *name, int value);

/**
 * @brief Looks up a symbol in the symbol table by name.
 * @param table Pointer to the SymbolTable.
 * @param name Name of the symbol to look up.
 * @return Pointer to the Symbol if found, otherwise NULL.
 */
Symbol *lookup_symbol(SymbolTable *table, const char *name);

/**
 * @brief Prints the contents of the symbol table for debugging purposes.
 * @param table Pointer to the SymbolTable to print.
 */
void print_symbol_table(SymbolTable *table);

// Add the free function here
void free_symbol_table(SymbolTable *table);

#endif // SYMTABLE_H
