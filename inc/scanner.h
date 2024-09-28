/**
 * @file scanner.h
 * @brief Header file for scanner.c
 * @author Martin Kandera (xkande00)
 */

#ifndef IFJ_COMPILER_SCANNER_H
#define IFJ_COMPILER_SCANNER_H

#include <stdio.h>  // Pro použití funkce printf

// Deklarace funkce pro výpis lexikálního chybového hlášení
void printLexicalError(const char* message, int line);

#endif // IFJ_COMPILER_SCANNER_H
