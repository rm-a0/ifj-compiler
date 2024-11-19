#ifndef TESTCG_H
#define TESTCG_H

#include "../inc/ast.h"  // Pridajte správnu cestu k definícii ASTNode
#include "../inc/error.h"
#include "../inc/generator.h"

void print_ast_to_file(ASTNode* program_node, int test_number);

#endif // TESTCG_H
