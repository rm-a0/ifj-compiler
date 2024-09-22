/**
 * @file error.h
 * @brief Header file containing return values for different error types
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef ERROR_H
#define ERROR_H

/**
 * @enum ErrorType
 * @brief Constants for different error types
 * 
 * @param NO_ERROR                          No error occurred (0).
 * @param LEXICAL_ERROR                     Error occurred during lexical analysis (1).
 * @param SYNTAX_ERROR                      Error occurred during syntax analysis (2).
 * @param SEMANTIC_ERROR_UNDEFINED          Undefined function or variable (3).
 * @param SEMANTIC_ERROR_PARAMS             Incorrect number/type of parameters or type mismatch/improper discard of return value (4).
 * @param SEMANTIC_ERROR_REDEF              Variable/function redefinition or assignment to non-modifiable variable (5).
 * @param SEMANTIC_ERROR_RETURN             Missing/excess expression in the return statement of the function (6).
 * @param SEMANTIC_ERROR_TYPE_COMPAT        Type compatibility error in expressions (e.g., arithmetic, string, relational) (7).
 * @param SEMANTIC_ERROR_TYPE_DERIVATION    Type inference issues (8).
 * @param SEMANTIC_ERROR_UNUSED_VAR         Unused variable within its scope (9).
 * @param OTHER_SEMANTIC_ERROR              Other unspecified semantic errors (10).
 * @param INTERNAL_ERROR                    Internal compiler error, not affected by input program (99).
 */
enum ErrorType { 
    NO_ERROR = 0,
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    SEMANTIC_ERROR_UNDEFINED = 3,
    SEMANTIC_ERROR_PARAMS = 4,
    SEMANTIC_ERROR_REDEF = 5,
    SEMANTIC_ERROR_RETURN = 6,
    SEMANTIC_ERROR_TYPE_COMPAT = 7,
    SEMANTIC_ERROR_TYPE_DERIVATION = 8,
    SEMANTIC_ERROR_UNUSED_VAR = 9,
    OTHER_SEMANTIC_ERROR = 10,
    INTERNAL_ERROR = 99
};

#endif // ERROR_H
