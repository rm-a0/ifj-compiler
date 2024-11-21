/**
 * @file ascii_lookup.h
 * @brief Header file containing lookup table for ascii characters
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef ASCII_LOOKUP_H
#define ASCII_LOOKUP_H

#define ASCII_CHARS 128

/**
 * @typedef int LookupTable[ASCII_CHARS]
 * @brief Lookup table with fixed size.
 * (ASCII_CHARS == 128)
 * 
 * @note Provides a clear and consistent interface for 
 * creating lookup tables indexed by ASCII characters. 
 * Ensures type safety by enforcing the correct table size, 
 * preventing accidental misuse of functions that require 
 * arrays of exactly 128 elements.
*/
typedef int LookupTable[ASCII_CHARS];

/**
 * @fn void init_lookup_table(LookupTable table)
 * @brief Initializes all inavlid elements to 0 (INVALID)
 * and all valid elements to its TokeType.
 * 
 * @param[out] table Lookup table
 * @return void
 * 
 * @note This function should be used only by lexer for checking
 * validity of single characters while scanning input.
*/
void init_lookup_table(LookupTable table);

#endif // ASCII_LOOKUP_H
