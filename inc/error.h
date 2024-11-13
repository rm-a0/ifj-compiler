/**
 * @file error.h
 * @brief Header file containing return values for different error types
 * @authors ...
 */

#ifndef ERROR_H
#define ERROR_H

/**
 * @enum ErrorType
 * @brief Constants for different error types.
 */
typedef enum { 
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
} ErrorType;

/**
 * @var ErrorType error_tracker
 * @brief Global variable across all files to keep track of different error types
 */
extern ErrorType error_tracker;

/**
 * @fn void set_error(ErrorType error)
 * @brief Sets global variable error_tracker to desired value
 * 
 * Error is set only if the error_tracker is set to NO_ERROR to
 * prevent overwriting previous errors.
 * 
 * @param[in] error Type of error to be set
 * @return void
 */
void set_error(ErrorType error);

/**
 * @fn ErrorType get_error(void)
 * @brief Retrieves the current error state.
 * 
 * @return The current ErrorType.
 */
ErrorType get_error(void);

/**
 * @fn void reset_error(void)
 * @brief Resets the error state to NO_ERROR.
 * 
 * @return void
 */
void reset_error(void);

#endif // ERROR_H
