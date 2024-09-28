/**
 * @file scanner.h
 * @brief Header file for scanner.c
 * @author Martin Kandera (xkande00)
 */
#include <stdio.h>
#include <stdbool.h>
#include "../inc/token.h"
#ifndef IFJ_COMPILER_SCANNER_H
#define IFJ_COMPILER_SCANNER_H

#include <stdio.h>  // Pro použití funkce printf

// Deklarace funkce pro výpis lexikálního chybového hlášení
void printLexicalError(const char* message, int line);

// Deklarace funkcí
int initScanner(const char* sourceCode);

Token getNextToken();

void printLexicalError(const char* message, int line);

extern bool devTool;

#endif // IFJ_COMPILER_SCANNER_H
