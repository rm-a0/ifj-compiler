#include "stack.h"
#include "symtable.h"
#include <assert.h>
#include <stddef.h>    // For NULL
#include <assert.h>    // For assert()
#include <stdlib.h>    // For malloc(), free()
#include <stdio.h>     // For printf()
#include <string.h>    // For strdup()


int main() {
    // Initialize RootStack and add a function to it
    RootStack *rootStack = init_root_stack();
    add_function(rootStack, "main");

    // Retrieve the function's ScopeStack and perform tests
    ScopeStack *mainStack = get_function_stack(rootStack, "main");

    // Test pushing frames and resizing
    for (int i = 0; i < 15; i++) {
        if (i % 2 == 0) {
            push_frame(mainStack, IF_COND_STATEMENT);
        } else {
            push_frame(mainStack, WHILE_STATEMENT);
        }
    }

    // Check that the scope stack resized correctly
    assert(mainStack->capacity >= 15);
    assert(mainStack->top == 14);

    // Add symbols to test symbol table resizing
    Frame *topFrame = top_frame(mainStack);
    for (int i = 0; i < 15; i++) {
        char symbolName[20];
        snprintf(symbolName, sizeof(symbolName), "var_%d", i);
        add_symbol(topFrame->symbolTable, symbolName, i);
    }

    // Verify symbol table resizing
    assert(topFrame->symbolTable->capacity >= 15);
    assert(topFrame->symbolTable->count == 15);

    // Lookup symbols and check their values
    for (int i = 0; i < 15; i++) {
        char symbolName[20];
        snprintf(symbolName, sizeof(symbolName), "var_%d", i);
        Symbol *symbol = lookup_symbol(topFrame->symbolTable, symbolName);
        assert(symbol != NULL);
        assert(symbol->value == i);
    }

    // Test deallocation
    free_symbol_table(topFrame->symbolTable);
    pop_frame(mainStack);

    // Print for debugging
    printf("Testing root stack with resizing and conditional frame types:\n");
    print_root_stack(rootStack);

    // Free all allocated memory
    for (int i = 0; i <= mainStack->top; i++) {
        free_symbol_table(mainStack->frames[i]->symbolTable);
        free(mainStack->frames[i]);
    }
    free(mainStack->frames);
    free(rootStack->functionNames[0]);
    free(rootStack->functionNames);
    free(rootStack->functions);
    free(rootStack);

    printf("All tests passed successfully.\n");
    return 0;
}
