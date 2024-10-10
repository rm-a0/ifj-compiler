// program ::= { top_level_decl }

// top_level_decl ::= fn_decl | const_decl | var_decl
// Declarations
FnDecl ::= "pub" "fn" IDENTIFIER "(" [ param_list ] ")" type block
ParamList → Param { "," Param }
Param → Identifier ":" Type

ConsDecl ::= "const" IDENTIFIER ":" type "=" expr ";"

VarDecl ::= "var" IDENTIFIER ":" type "=" expr ";"

BuildInFunc ::= IDENTIFIER "." IDENTIFIER "(" [ ArgList ] ")" 

FnCall ::= Identifier "(" [ ArgList ] ")" 
ArgList → Expr { "," Expr }


type ::= "i32" | "u8" | "f64" | "void"

TypeIdent ::= "?" type

block ::= "{" { stmt | IfStmt | WhileStmt  } "}"

stmt ::= Assignment | FnCall ";"

Assignment → type Identifier "=" Expr | FnCall ";"

// Expressions

Expr → BinaryExpr | PrimaryExpr
BinaryExpr → Expr BinOp Expr
PrimaryExpr → Identifier | Literal | "(" Expr ")"
BinOp → "+" | "-" | "*" | "/" | "&&" | "||" | "==" | "!=" | "<" | ">" | "<=" | ">="
Literal → IntLiteral | FloatLiteral | StringLiteral

// Condition

IfStmt → "if" "(" Expr ")" Block ElsePart
ElsePart → "else" Block | ε

// Cycle

WhileStmt → "while" "(" Expr ")" Block




