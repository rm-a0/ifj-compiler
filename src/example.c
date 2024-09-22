/**
 * @file token.c
 * @brief Description of a file
 * @authors Author (xlogin)
*/

#include <stdio.h>
#include "example.h"

void function_name(int argument) {
    if (argument < 0) {
        function_name(argument - 1);    
    }
    else {
        return;
    }
}

int main() {
    StructName custom_name;
    function_name(9);
    return 0;
}