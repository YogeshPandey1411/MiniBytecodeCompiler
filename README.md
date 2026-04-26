# Mini Bytecode Compiler & Virtual Machine

A four-stage compiler pipeline built in C (C99) for the Compiler Design course.  
The project takes a custom high-level language, passes it through every classical
compiler phase, and executes the resulting bytecode on a custom stack-based VM.

---

## Language Features

| Feature            | Syntax example                          |
|--------------------|-----------------------------------------|
| Variable declaration | `let x = 10;`                         |
| Assignment           | `x = x + 1;`                         |
| Arithmetic           | `+ - * /`                             |
| Comparison           | `== != < > <= >=`                     |
| Conditional          | `if (cond) { ... } else { ... }`      |
| Loop                 | `while (cond) { ... }`               |
| Output               | `print expr;`                         |
| Comments             | `// single-line comment`              |

---

## Project Structure

```
mini_compiler/
├── include/
│   ├── common.h      — shared types: Token, ASTNode, Instruction, CodeObject
│   ├── lexer.h
│   ├── parser.h
│   ├── codegen.h
│   └── vm.h
├── src/
│   ├── main.c        — interactive menu + pipeline runner
│   ├── lexer.c       — Phase 1: tokeniser
│   ├── parser.c      — Phase 2: recursive-descent parser → AST
│   ├── codegen.c     — Phase 3: AST walker → bytecode
│   └── vm.c          — Phase 4: stack-based virtual machine
├── tests/
│   ├── test_arithmetic.src
│   ├── test_if_else.src
│   └── test_while.src
├── examples/
│   ├── factorial.src
│   └── fizz_check.src
├── Makefile
└── README.md
```

---

## Build & Run

```bash
# Build
make

# Run interactive menu
./compiler

# Compile a source file directly
./compiler examples/factorial.src

# Run all tests
make test

# Clean build artifacts
make clean
```

---

## Pipeline Phases

### Phase 1 — Lexer  (`src/lexer.c`)
Reads raw source text and converts it into a flat list of **tokens**.  
Each token has a type (NUMBER, IDENT, keyword, operator, …) and a line number.  
- Uses a single-pass character scan.  
- Skips whitespace and `//` comments.  
- Reports unknown characters with a line-number error.

### Phase 2 — Parser  (`src/parser.c`)
Converts the token list into an **Abstract Syntax Tree (AST)** using
recursive-descent parsing.

Grammar (simplified):
```
program   → stmt*
stmt      → let_stmt | assign_stmt | print_stmt | if_stmt | while_stmt
expr      → comparison
comparison→ add_sub ( ('<'|'>'|'<='|'>='|'=='|'!=') add_sub )?
add_sub   → mul_div ( ('+'|'-') mul_div )*
mul_div   → primary ( ('*'|'/') primary )*
primary   → NUMBER | IDENT | '(' expr ')'
```

### Phase 3 — Code Generator  (`src/codegen.c`)
Walks the AST and emits a flat array of **bytecode instructions**.  
Handles:
- `PUSH` / `LOAD` / `STORE` for values and variables  
- `ADD` `SUB` `MUL` `DIV` for arithmetic  
- `CMP_*` for comparisons  
- `JMP` / `JZ` with **back-patching** for if/else and while loops  

### Phase 4 — Virtual Machine  (`src/vm.c`)
A simple **stack-based interpreter** that executes the bytecode.  
- Maintains an integer stack and a variable store (name → int).  
- Instruction pointer advances linearly; jump instructions redirect it.  
- Reports runtime errors (division by zero, undefined variable, etc.).

---

## Team Division of Work

| Member | Component                     |
|--------|-------------------------------|
| TL     | `main.c` + `common.h` + build system |
| M2     | `lexer.c` / `lexer.h`         |
| M3     | `parser.c` / `parser.h`       |
| M4     | `codegen.c` + `vm.c`          |

---

## Sample Output

Running `./compiler examples/factorial.src` produces:

```
PHASE 1 : LEXER  (Tokeniser)
  LINE  TYPE          VALUE
  ...tokens listed...
  ✓ Lexer produced N token(s).

PHASE 2 : PARSER  (AST Builder)
  BLOCK (4 stmts)
    ASSIGN(n)  NUM(5)
    ASSIGN(result)  NUM(1)
    ...
  ✓ Parser built AST successfully.

PHASE 3 : CODE GENERATOR  (Bytecode)
  ADDR  OPCODE    OPERAND
  0000  PUSH      5
  0001  STORE     n
  ...
  ✓ Code generator emitted N instruction(s).

PHASE 4 : VIRTUAL MACHINE  (Execution)
  Program Output:
  ─────────────────────────────────────────
  120
  ─────────────────────────────────────────
  ✓ VM finished execution.
```
