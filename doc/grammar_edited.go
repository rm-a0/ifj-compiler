/**
 * @file grammar.go
 * @brief File containing LL grammar for IFJ24 language in EBNF
 * @authors Simon Bobko (xbobkos00) Michal Repcik (xrepcim00)
*/

/*
	"" 	- terminal
	<> 	- non-terminal
	[] 	- mandatory
	* 	- repetition
	|  	- or
*/

// Root grammat definitions
<program> 			::= <top_level_decl>
<top_level_decl> 	::= <fn_decl> | <const_decl> | <var_decl> | <prolog>

// Most used main components
<identifier> 		::= [a-zA-Z] [_a-zA-Z0-9]*
<integer> 			::= [0-9]+
<float>				::= <integer> "." [("e" | "E") ("+" | "-")] <integer>
<data_type> 		::= "void" | "u8" | "i32" | "f64"

// Hardcoded prolog
<prolog> 			::= "const" "ifj" "=" "@import" "(" "ifj24.zig" ")" ";"

// Funcion declaration
<fn_decl> 			::= "pub" "fn" <identifier> "(" [param_list] ")" <data_type> <block>
<param_list>		::= <param> ("," <param>)* // after last param there can be ','
<param>				::= <identifier> ":" <data_type>

// Constant variable declaration
<const_decl> 		::= "const" <identifier> [":" <data_type>] "=" <expression> ";"
<expression>        ::= <term> (("+" | "-") <term>)*
<term>              ::= <factor> (("*" | "/") <factor>)*
<factor>            ::= <number> | <identifier> | "(" <expression> ")"
<number>			::= <integer> | <float>