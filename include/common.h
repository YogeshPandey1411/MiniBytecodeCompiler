#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ── Color codes (disabled for Windows compatibility) ───────────────── */
#define COL_RESET   ""
#define COL_BOLD    ""
#define COL_RED     ""
#define COL_GREEN   ""
#define COL_YELLOW  ""
#define COL_CYAN    ""
#define COL_MAGENTA ""
#define COL_BLUE    ""

/* ── Limits ──────────────────────────────────────────────────────────── */
#define MAX_TOKENS      512
#define MAX_IDENT_LEN   64
#define MAX_CODE        1024
#define MAX_STACK       256
#define MAX_VARS        64
#define MAX_SOURCE_LEN  4096

/* ── Token types ─────────────────────────────────────────────────────── */
typedef enum {
    /* Literals & identifiers */
    TOK_NUMBER, TOK_IDENT,
    /* Keywords */
    TOK_LET, TOK_PRINT, TOK_IF, TOK_ELSE, TOK_WHILE,
    /* Operators */
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQ, TOK_EQEQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    /* Delimiters */
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_SEMI,
    /* Special */
    TOK_EOF, TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char      value[MAX_IDENT_LEN];
    int       line;
} Token;

/* ── AST node types ──────────────────────────────────────────────────── */
typedef enum {
    NODE_NUM, NODE_VAR,
    NODE_BINOP,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_IF,
    NODE_WHILE,
    NODE_BLOCK
} NodeType;

typedef struct ASTNode {
    NodeType type;

    /* For NUM */
    int num_value;

    /* For VAR / ASSIGN */
    char name[MAX_IDENT_LEN];

    /* For BINOP */
    char op;          /* '+' '-' '*' '/' '<' '>' 'l'(<=) 'g'(>=) 'e'(==) 'n'(!=) */

    /* Children */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;     /* for IF / WHILE */
    struct ASTNode *then_br;
    struct ASTNode *else_br;

    /* For BLOCK: array of statements */
    struct ASTNode **stmts;
    int              stmt_count;
} ASTNode;

/* ── Bytecode opcodes ────────────────────────────────────────────────── */
typedef enum {
    OP_PUSH,    /* push immediate int */
    OP_LOAD,    /* load variable onto stack */
    OP_STORE,   /* pop stack → variable */
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_CMP_EQ, OP_CMP_NEQ,
    OP_CMP_LT,  OP_CMP_GT,
    OP_CMP_LE,  OP_CMP_GE,
    OP_PRINT,   /* pop & print top of stack */
    OP_JMP,     /* unconditional jump */
    OP_JZ,      /* jump if zero (false) */
    OP_HALT
} Opcode;

typedef struct {
    Opcode opcode;
    int    operand;       /* index into constant pool or var table, or jump target */
    char   var_name[MAX_IDENT_LEN];  /* for LOAD / STORE */
} Instruction;

/* ── Code object (output of code generator) ─────────────────────────── */
typedef struct {
    Instruction code[MAX_CODE];
    int         code_len;
} CodeObject;

/* ── Helper: print a banner for each phase ───────────────────────────── */
static inline void print_phase_banner(const char *phase, const char *color) {
    (void)color;
    printf("\n==== %s ====\n", phase);
}

#endif /* COMMON_H */
