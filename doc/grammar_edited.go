/**
 * @file grammar.go
 * @brief File containing LL grammar for IFJ24 language in EBNF
 * @authors Simon Bobko (xbobkos00) Michal Repcik (xrepcim00)
*/

/*
	"" 	- terminal
	<> 	- non-terminal
	[] 	- optional
	* 	- repetition (0 or more (optional))
	|  	- or
*/

// Root grammat definitions
<<<<<<< HEAD
<program> 			::= <top_level_decl>*
<top_level_decl> 	::= <fn_decl> | <const_decl> | <var_decl> | <prolog>

// Most used main components
<identifier> 		::= [a-zA-Z] [_a-zA-Z0-9]*
<integer> 			::= [0-9]+
<float>				::= <integer> "." [("e" | "E") ("+" | "-")] <integer>
<string_literal>    ::= [^]* // anything can be inside string
<data_type> 		::= ["?"] ("void" | "u8" | "i32" | "f64" | "[]u8") // optional '?' with every use of data type
=======
<program> 			::= <prolog> | <top_level_decl>*
<top_level_decl> 	::= <fn_decl> | <const_decl> | <var_decl>

// Most used main components
<identifier> 		::= [a-zA-Z] [_a-zA-Z0-9]* // TOKEN_IDENTIFIER
<integer> 			::= [0-9]+ // TOKEN_INTEGER
<float>				::= <integer> "." [("e" | "E") ("+" | "-")] <integer> // TOKEN_FLOAT
<string_literal>    ::= [^]* // TOKEN_STRING
<data_type> 		::= ["?"] ("void" | "u8" | "i32" | "f64" | "[]u8") // optional '?' with every use of data type (fix probably)
>>>>>>> parser

// Hardcoded prolog
<prolog> 			::= "const" "ifj" "=" "@import" "(" "ifj24.zig" ")" ";"

// Funcion declaration
<<<<<<< HEAD
<fn_decl> 			::= "pub" "fn" <identifier> "(" [param_list] ")" <data_type> <block>
=======
<fn_decl> 			::= "pub" "fn" <identifier> "(" [<param_list>] ")" <data_type> <block>
>>>>>>> parser
<param_list>		::= <param> ("," <param>)* // after last param there can be ','
<param>				::= <identifier> ":" <data_type>

// Constant and variable declaration
<const_decl> 		::= "const" <identifier> [":" <data_type>] "=" <expression> ";"
<var_decl>			::= "var" <identifier> [":" <data_type>] "=" <expression> ";"

<<<<<<< HEAD
// Expression (probably the most crucial part)
=======
// Expression (most important part)
>>>>>>> parser
<expression>        ::= <term> (("+" | "-") <term>)*
<term>              ::= <factor> (("*" | "/") <factor>)*
<factor>            ::= <number> | <identifier> | <fn_call> | <string_literal> | "(" <expression> ")"
<number>			::= <integer> | <float>

// Block
<block>				::= "{" <statement>* "}"
<statement> 		::=	<const_decl> | <var_decl> | <assignment> | <fn_call_statement> | <if_statement> | <while_statement> | <return_statement>

// Assignment
<assignment> 		::= <identifier> "=" <expression> ";"

<<<<<<< HEAD
// Function call (varaint used inside expressions)
<fn_call>			::= (<built_in_fn> | <identifier>) "(" [<expression> ("," <expression>)*] ")"
// Built in function call

// Function call statement (variant with ; terminator at the end)
<fn_call_statement> ::= (<built_in_fn> | <identifier>) "(" [<expression> ("," <expression>)*] ")" ";"

// Built in functions
<built_in_fn> 		::= "ifj" "." ("write" | "readstr" | "readi32" | "readf64" | "i2f" | "f2i" | "string" | "length" | "concat" | "substring" | "strcmp" | "ord" | "chr")

// If statement
<if_statement>		::= "if" "(" <expression> [<binary_operator> <expression>] ")" ["|" <identifier> "|"] <block> ["else" <block>]

// While statement
<while_statement> 	::= "while" "(" <expression> [<binary_operator> <expression>] ")" ["|" <identifier> "|"] <block>
=======
// Function call (inside expression) and function call statement (inside block)
<fn_call>			::= <fn_identifier> "(" [<arg_list>] ")"
<fn_call_statement> ::= <fn_call> ";"
<fn_identifier> 	::= <built_in_fn> | <identifier>
<arg_list>			::= <expression> ("," <expression>)*
<built_in_fn> 		::= "ifj" "." ("write" | "readstr" | "readi32" | "readf64" | "i2f" | "f2i" | "string" | "length" | "concat" | "substring" | "strcmp" | "ord" | "chr")

// If and while statements (can only be 1 or 2 expression ?)
<if_statement>		::= "if" "(" <expression> [<bin_expression>] ")" [<element_bind>] <block> ["else" <block>]
<while_statement> 	::= "while" "(" <expression> [<bin_expression>] ")" [<element_bind>] <block>
<bin_expression> 	::= <binary_operator> <expression>
<element_bind> 		::= "|" <identifier> "|"
>>>>>>> parser

// Return statement
<return_statement> 	::= "return" [<expression>] ";"

// Binary operators
<<<<<<< HEAD
<binary_operator> 	::= "==" | "!=" | "<=" | ">=" | ">" | "<"
=======
<binary_operator> 	::= "==" | "!=" | "<=" | ">=" | ">" | "<"
>>>>>>> parser
