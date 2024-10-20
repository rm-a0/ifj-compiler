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

2. <top_level_decl_list>::= <top_level_decl> <top_level_decl_list 
3. <top_level_decl_list	::= ε

4. <top_level_decl> 	::= <fn_decl>
5. <top_level_decl		::= <const_decl>
6. <top_level_decl		::= <var_decl>

// Most used main components
7. <data_type> 		::= <type_prefix> <type>
8. <type>			::= "void" | "u8" | "i32" | "f64" | "[]u8"
9. <type_prefix>	::= "?" 
10.<type_prefix>	::= ε

// Funcion declaration
11. <fn_decl> 			::= "pub" "fn" identifier "(" <param_list> ")" <data_type> <block>
12. <param_list> 		::= <param> <param_list1>
13. <param_list>		::= ε
14. <param_list1> 		::= "," <param> <param_list1>
15. <param_list1>		::= ε
16.	<param> 	 		::= identifier ":" <data_type>


// Constant and variable declaration
17. <const_decl> 		::= "const" identifier <type_assign> "=" <expression> ";"
18. <var_decl>			::= "var" identifier <type_assign> "=" <expression> ";"
19. <type_assign>		::= ":" <data_type>
20. <type_assign>		::= ε

// Expression (probably the most crucial part)
21. <expression>     ::= <term> <expression1>
22. <expression1>    ::= "+" <term> <expression1>
23.	<expression1>	 ::= "-" <term> <expression1> 
24. <expression1>	 ::= ε

25. <term>           ::= <factor> <term1>
26. <term1>          ::= "*" <factor> <term1>
27. <term1>			 ::= "/" <factor> <term1>
28. <term1>			 ::= ε

29. <factor>         ::= <number>
30. <factor>		 ::= identifier <factor_suffix>
31. <factor>		 ::= string_literal
32. <factor>		 ::= "(" <expression> ")"

33. <factor_suffix>	 ::= "(" <arg_list> ")"
34. <factor_suffix>	 ::= ε

35. <number>         ::= integer
36. <number>		 ::= float


// Block
37. <block>				::= "{" <statement_list> "}"
38. <statement_list> 	::= <statement> <statement_list1> 
39. <statement_list1>	::= <statement> <statement_list1>
40. <statement_list1>	::= ε

41. <statement> 		::=	<const_decl> 
42. <statement> 		::= <var_decl>
43. <statement>			::= <assignment>
44. <statement>			::= <if_statement>
45. <statement>			::= <while_statement>
46. <statement>			::= <return_statement>

// Assignment
47. <assignment> 		::= identifier "=" <assignment1> ";"
48. <assignment1>		::= <expression> 
49. <assignment1>		::= <built_in_fn>


// Function call (inside expression) and function call statement (inside block)
50. <arg_list> 			::= <argument> 
51. <arg_list>			::= ε
52. <argument>			::= <factor> <argument1>
53. <argument1>			::= "," <argument1>
54. <argument1>			::= ε
55. <built_in_fn> 		::= "ifj" "." <built_in_string> "(" <arg_list> ")"
56. <built_in_string>	::= "write" | "readstr" | "readi32" | "readf64" | "i2f" | "f2i" | "string" | "length" | "concat" | "substring" | "strcmp" | "ord" | "chr"

// If and while statements (can only be 1 or 2 expression ?)
57. <if_statement>		::= "if" "(" <expression> <bin_expression> ")" <element_bind> <block> <else_statement>
58. <else_statement>	::= "else" <block>
59. <else_statement>	::= ε
60. <while_statement> 	::= "while" "(" <expression> <bin_expression> ")" <element_bind> <block>
61. <bin_expression> 	::= <binary_operator> <expression>
62. <bin_expression>	::= ε
63. <element_bind> 		::= "|" identifier "|" 
64. <element_bind>		::= ε

// Return statement
65. <return_statement> 	::= "return" <return_statement1> ";"
66. <return_statement1>	::= <expression>
67. <return_statement1	::= ε

// Binary operators
68. <binary_operator> 	::= "==" | "!=" | "<=" | ">=" | ">" | "<"
