/**
 * @file grammar.go
 * @brief File containing LL grammar for IFJ24 language
 * @authors Michal Repcik (xrepcim00)
*/

<program>               := <statement>*
<statement>             := <declaration> | <expression> | <if> | <while> | <block> | <return>

<program>               ::= <statement>*
<statement>             ::= <declaration> | <expression_statement> | <if_statement> | <while_statement> | <return_statement> | <block_statement>
<declaration>           ::= <var_declaration> | <func_declaration>
<var_declaration>       ::= "var" <identifier> ":" <type> ["=" <expression>] ";"
<func_declaration>      ::= "fn" <identifier> "(" [<param_list>] ")" "{" <block_statement> "}"
<param_list>           ::= <parameter> ("," <parameter>)*
<parameter>             ::= <identifier> ":" <type>
<type>                  ::= "i32" | "i64" | "f32" | "f64" | "bool" | "void" | <identifier>
<block_statement>       ::= "{" <statement>* "}"
<expression_statement>  ::= <expression> ";"
<if_statement>          ::= "if" "(" <expression> ")" <block_statement> ["else" <block_statement>]
<while_statement>       ::= "while" "(" <expression> ")" <block_statement>
<return_statement>      ::= "return" [<expression>] ";"
<expression>            ::= <term> (("+" | "-") <term>)*
<term>                  ::= <factor> (("*" | "/") <factor>)*
<factor>                ::= <number> | <identifier> | "(" <expression> ")"
<identifier>            ::= [a-zA-Z_] [a-zA-Z0-9_]*
<number>                ::= [0-9]+