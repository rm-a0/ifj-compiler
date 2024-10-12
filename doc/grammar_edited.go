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
<program> 			::= <top_level_decl>*
<top_level_decl> 	::= <fn_decl> | <const_decl> | <var_decl> | <prolog>

// Most used main components
<identifier> 		::= [a-zA-Z] [_a-zA-Z0-9]*
<integer> 			::= [0-9]+
<float>				::= <integer> "." [("e" | "E") ("+" | "-")] <integer>
<string_literal>    ::= [^]* // anything can be inside string
<data_type> 		::= ["?"] ("void" | "u8" | "i32" | "f64" | "[]u8") // optional '?' with every use of data type

// Hardcoded prolog
<prolog> 			::= "const" "ifj" "=" "@import" "(" "ifj24.zig" ")" ";"

// Funcion declaration
<fn_decl> 			::= "pub" "fn" <identifier> "(" [param_list] ")" <data_type> <block>
<param_list>		::= <param> ("," <param>)* // after last param there can be ','
<param>				::= <identifier> ":" <data_type>

// Constant and variable declaration
<const_decl> 		::= "const" <identifier> [":" <data_type>] "=" <expression> ";"
<var_decl>			::= "var" <identifier> [":" <data_type>] "=" <expression> ";"

// Expression (probably the most crucial part)
<expression>        ::= <term> (("+" | "-") <term>)*
<term>              ::= <factor> (("*" | "/") <factor>)*
<factor>            ::= <number> | <identifier> | <fn_call> | <string_literal> | "(" <expression> ")"
<number>			::= <integer> | <float>

// Block
<block>				::= "{" <statement>* "}"
<statement> 		::=	<const_decl> | <var_decl> | <assignment> | <fn_call_statement> | <if_statement> | <while_statement> | <return_statement>

// Assignment
<assignment> 		::= <identifier> "=" <expression> ";"

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

// Return statement
<return_statement> 	::= "return" [<expression>] ";"

// Binary operators
<binary_operator> 	::= "==" | "!=" | "<=" | ">=" | ">" | "<"