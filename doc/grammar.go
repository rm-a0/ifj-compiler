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
1. <program> 			::= <top_level_decl_list>
2. <top_level_decl_list>::= <top_level_decl> <top_level_decl_list | ε
3. <top_level_decl> 	::= <fn_decl> | <const_decl> | <var_decl>

// Most used main components
4. <data_type> 		::= <type_prefix> <type>
5. <type>			::= "void" | "u8" | "i32" | "f64" | "[]u8"
6. <type_prefix>	::= "?" | ε

// Funcion declaration
7. <fn_decl> 			::= "pub" "fn" identifier "(" <param_list> ")" <data_type> <block>
8. <param_list> 		::= <param> <param_list1> | ε
9. <param_list1> 		::= "," <param> <param_list1> | ε
10.<param> 	 			::= identifier ":" <data_type>


// Constant and variable declaration
11. <const_decl> 		::= "const" identifier <type_assign> "=" <expression> ";"
12. <var_decl>			::= "var" identifier <type_assign> "=" <expression> ";"
13. <type_assign>		::= ":" <data_type> | ε

// Expression (probably the most crucial part)
14. <expression>     ::= <term> <expression1>
15. <expression1>    ::= "+" <term> <expression1> | "-" <term> <expression1> | ε

16. <term>           ::= <factor> <term1>
17. <term1>          ::= "*" <factor> <term1> | "/" <factor> <term1> | ε

18. <factor>         ::= <number> | identifier <factor_suffix> | string_literal | "(" <expression> ")"
19. <factor_suffix>	 ::= "(" <arg_list> ")" | ε

20. <number>         ::= integer | float


// Block
21. <block>				::= "{" <statement_list> "}"
22. <statement_list> 	::= <statement> <statement_list1> 
23. <statement_list1>	::= <statement> <statement_list1> | ε
24. <statement> 		::=	<const_decl> | <var_decl> | <assignment> | <if_statement> | <while_statement> | <return_statement>

// Assignment
25. <assignment> 		::= identifier "=" <assignment1> ";"
26. <assignment1>		::= <expression> | <built_in_fn>


// Function call (inside expression) and function call statement (inside block)
27. <arg_list> 			::= <argument> | ε
28. <argument>			::= <factor> <argument1>
29. <argument1>			::= "," <argument1> | ε
30. <built_in_fn> 		::= "ifj" "." <built_in_string> "(" <arg_list> ")"
31. <built_in_string>	::= "write" | "readstr" | "readi32" | "readf64" | "i2f" | "f2i" | "string" | "length" | "concat" | "substring" | "strcmp" | "ord" | "chr"

// If and while statements (can only be 1 or 2 expression ?)
32. <if_statement>		::= "if" "(" <expression> <bin_expression> ")" <element_bind> <block> <else_statement>
33. <else_statement>	::= "else" <block> | ε
34. <while_statement> 	::= "while" "(" <expression> <bin_expression> ")" <element_bind> <block>
35. <bin_expression> 	::= <binary_operator> <expression> | ε
36. <element_bind> 		::= "|" identifier "|" | ε

// Return statement
37. <return_statement> 	::= "return" <return_statement1> ";"
38. <return_statement1>	::= <expression> | ε

// Binary operators
39. <binary_operator> 	::= "==" | "!=" | "<=" | ">=" | ">" | "<"
