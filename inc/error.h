/**
 * @file error.h
 * @brief Header file containing return values for different error types
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef ERROR_H
#define ERROR_H

/**
 * @enum ErrorType
 * @brief Constants for different error types.
*/
enum ErrorType { 
    NO_ERROR = 0,                           ///< No error occurred.
    LEXICAL_ERROR = 1,                      ///< Error occurred during lexical analysis.
    SYNTAX_ERROR = 2,                       ///< Error occurred during syntax analysis.
    SEMANTIC_ERROR_UNDEFINED = 3,           ///< Undefined function or variable.
    SEMANTIC_ERROR_PARAMS = 4,              ///< Incorrect number/type of parameters or type mismatch/improper discard of return value.
    SEMANTIC_ERROR_REDEF = 5,               ///< Variable/function redefinition or assignment to non-modifiable variable.
    SEMANTIC_ERROR_RETURN = 6,              ///< Missing/excess expression in the return statement of the function.
    SEMANTIC_ERROR_TYPE_COMPAT = 7,         ///< Type compatibility error in expressions (e.g., arithmetic, string, relational).
    SEMANTIC_ERROR_TYPE_DERIVATION = 8,     ///< Type inference issues.
    SEMANTIC_ERROR_UNUSED_VAR = 9,          ///< Unused variable within its scope.
    OTHER_SEMANTIC_ERROR = 10,              ///< Other unspecified semantic errors.
    INTERNAL_ERROR = 99                     ///< Internal compiler error, not affected by input program.
};

#endif // ERROR_H
