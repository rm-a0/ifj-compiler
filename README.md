## IFJ24 Compiler

Custom compiler written in C for a subset of the Zig programming language.

## Overview
This project is an implementation of a compiler for the IFJ24 imperative programming language. It includes lexical analysis, parsing, semantic analysis, and code generation. The compiler follows an LL(1) parsing approach and generates target code with optimizations.


## Features
- **Lexical Analysis**: FSM-based lexer for tokenizing source code
- **Parsing**: Recursive descent parser
- **Semantic Analysis**: Symbol table and type checking
- **Code Generation**: Optimized target code output

## Team
- **Michal Repčík (xrepcim00)** – Lexer, AST, Parser
- **Alex Marinica (xmarina00)** – Symbol table, Semantic analysis
- **Šimon Bobko (xbobkos00)** – Grammar, LL parsing, Expression parser, Testing
- **Martin Kandera (xkande00)** – Code generation, Testing

## Build & Run
1. Clone the repository:
   ```sh
   git clone https://github.com/your-repo/ifj24-compiler.git
   cd ifj24-compiler
   ```

## Documentation
For more detailed documentation, use the documentation in the `doc` folder.  
Refer to `doc/documentation.tex` for in-depth details.

## Final Evaluation
- Lexical Analysis - 85%
- Syntax Analysis - 95%
- Semantic Analysis - 81%
- Code Generation (basic tests) - 68%
- Code Generation (advanced tests) - 42%
- Code Generation (complex tests) - 22%
- FUNEXP extension - 90%
- __Total__ - 65%
