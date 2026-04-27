#include "parser.h"

/* ── Parser state ────────────────────────────────────────────────────── */
static const Token *g_tokens;
static int          g_pos;
static int          g_count;

/* ── Helpers ─────────────────────────────────────────────────────────── */
static const Token *peek(void) {
    return &g_tokens[g_pos];
}

static const Token *advance(void) {
    const Token *t = &g_tokens[g_pos];
    if (t->type != TOK_EOF) g_pos++;
    return t;
}

static int check(TokenType t) {
    return peek()->type == t;
}

static int match(TokenType t) {
    if (check(t)) { advance(); return 1; }
    return 0;
}

static const Token *expect(TokenType t, const char *msg) {
    if (!check(t)) {
        fprintf(stderr, COL_RED "[Parser] Error at line %d: expected %s, got '%s'\n" COL_RESET,
                peek()->line, msg, peek()->value);
        return NULL;
    }
    return advance();
}

/* ── AST node allocation ──────────────────────────────────────────────── */
static ASTNode *new_node(NodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    if (!n) { fprintf(stderr, COL_RED "[Parser] Out of memory\n" COL_RESET); exit(1); }
    n->type = type;
    return n;
}

/* ── Forward declarations ────────────────────────────────────────────── */
static ASTNode *parse_stmt(void);
static ASTNode *parse_expr(void);
static ASTNode *parse_comparison(void);
static ASTNode *parse_add_sub(void);
static ASTNode *parse_mul_div(void);
static ASTNode *parse_primary(void);

/* ── Grammar rules ───────────────────────────────────────────────────── */

/*  block → '{' stmt* '}'  */
static ASTNode *parse_block(void) {
    if (!expect(TOK_LBRACE, "'{'")) return NULL;

    ASTNode *block = new_node(NODE_BLOCK);
    /* Use a small dynamic array for statements */
    int capacity = 16;
    block->stmts = malloc(capacity * sizeof(ASTNode*));
    block->stmt_count = 0;

    while (!check(TOK_RBRACE) && !check(TOK_EOF)) {
        ASTNode *s = parse_stmt();
        if (!s) return NULL;
        if (block->stmt_count == capacity) {
            capacity *= 2;
            block->stmts = realloc(block->stmts, capacity * sizeof(ASTNode*));
        }
        block->stmts[block->stmt_count++] = s;
    }

    if (!expect(TOK_RBRACE, "'}'")) return NULL;
    return block;
}

/*  stmt → let_stmt | print_stmt | if_stmt | while_stmt | assign_stmt  */
static ASTNode *parse_stmt(void) {

    /* let x = expr ; */
    if (check(TOK_LET)) {
        advance();
        const Token *id = expect(TOK_IDENT, "variable name");
        if (!id) return NULL;
        if (!expect(TOK_EQ, "'='")) return NULL;
        ASTNode *val = parse_expr();
        if (!val) return NULL;
        if (!expect(TOK_SEMI, "';'")) return NULL;

        ASTNode *n = new_node(NODE_ASSIGN);
        strncpy(n->name, id->value, MAX_IDENT_LEN - 1);
        n->right = val;
        return n;
    }

    /* print expr ; */
    if (check(TOK_PRINT)) {
        advance();
        ASTNode *val = parse_expr();
        if (!val) return NULL;
        if (!expect(TOK_SEMI, "';'")) return NULL;

        ASTNode *n = new_node(NODE_PRINT);
        n->right = val;
        return n;
    }

    /* if ( expr ) block [else block] */
    if (check(TOK_IF)) {
        advance();
        if (!expect(TOK_LPAREN, "'('")) return NULL;
        ASTNode *cond = parse_expr();
        if (!cond) return NULL;
        if (!expect(TOK_RPAREN, "')'")) return NULL;
        ASTNode *then_br = parse_block();
        if (!then_br) return NULL;

        ASTNode *else_br = NULL;
        if (match(TOK_ELSE)) {
            else_br = parse_block();
            if (!else_br) return NULL;
        }

        ASTNode *n = new_node(NODE_IF);
        n->cond    = cond;
        n->then_br = then_br;
        n->else_br = else_br;
        return n;
    }

    /* while ( expr ) block */
    if (check(TOK_WHILE)) {
        advance();
        if (!expect(TOK_LPAREN, "'('")) return NULL;
        ASTNode *cond = parse_expr();
        if (!cond) return NULL;
        if (!expect(TOK_RPAREN, "')'")) return NULL;
        ASTNode *body = parse_block();
        if (!body) return NULL;

        ASTNode *n = new_node(NODE_WHILE);
        n->cond    = cond;
        n->then_br = body;
        return n;
    }

    /* x = expr ; */
    if (check(TOK_IDENT) && g_pos + 1 < g_count && g_tokens[g_pos + 1].type == TOK_EQ) {
        const Token *id = advance();
        advance(); /* consume '=' */
        ASTNode *val = parse_expr();
        if (!val) return NULL;
        if (!expect(TOK_SEMI, "';'")) return NULL;

        ASTNode *n = new_node(NODE_ASSIGN);
        strncpy(n->name, id->value, MAX_IDENT_LEN - 1);
        n->right = val;
        return n;
    }

    fprintf(stderr, COL_RED "[Parser] Unexpected token '%s' at line %d\n" COL_RESET,
            peek()->value, peek()->line);
    return NULL;
}

/* expr → comparison */
static ASTNode *parse_expr(void) {
    return parse_comparison();
}

/* comparison → add_sub (('<'|'>'|'<='|'>='|'=='|'!=') add_sub)? */
static ASTNode *parse_comparison(void) {
    ASTNode *left = parse_add_sub();
    if (!left) return NULL;

    char op_char = 0;
    TokenType tt = peek()->type;
    if      (tt == TOK_LT)   op_char = '<';
    else if (tt == TOK_GT)   op_char = '>';
    else if (tt == TOK_LE)   op_char = 'l';   /* <= */
    else if (tt == TOK_GE)   op_char = 'g';   /* >= */
    else if (tt == TOK_EQEQ) op_char = 'e';   /* == */
    else if (tt == TOK_NEQ)  op_char = 'n';   /* != */

    if (op_char) {
        advance();
        ASTNode *right = parse_add_sub();
        if (!right) return NULL;
        ASTNode *n = new_node(NODE_BINOP);
        n->op    = op_char;
        n->left  = left;
        n->right = right;
        return n;
    }
    return left;
}

/* add_sub → mul_div (('+' | '-') mul_div)* */
static ASTNode *parse_add_sub(void) {
    ASTNode *left = parse_mul_div();
    if (!left) return NULL;

    while (check(TOK_PLUS) || check(TOK_MINUS)) {
        char op = peek()->type == TOK_PLUS ? '+' : '-';
        advance();
        ASTNode *right = parse_mul_div();
        if (!right) return NULL;
        ASTNode *n = new_node(NODE_BINOP);
        n->op = op; n->left = left; n->right = right;
        left = n;
    }
    return left;
}

/* mul_div → primary (('*' | '/') primary)* */
static ASTNode *parse_mul_div(void) {
    ASTNode *left = parse_primary();
    if (!left) return NULL;

    while (check(TOK_STAR) || check(TOK_SLASH) || check(TOK_PERCENT)) {
        char op = peek()->type == TOK_STAR ? '*' : (peek()->type == TOK_SLASH ? '/' : '%');
        advance();
        ASTNode *right = parse_primary();
        if (!right) return NULL;
        ASTNode *n = new_node(NODE_BINOP);
        n->op = op; n->left = left; n->right = right;
        left = n;
    }
    return left;
}

/* primary → NUMBER | IDENT | '(' expr ')' */
static ASTNode *parse_primary(void) {
    if (check(TOK_NUMBER)) {
        const Token *t = advance();
        ASTNode *n = new_node(NODE_NUM);
        n->num_value = atoi(t->value);
        return n;
    }
    if (check(TOK_IDENT)) {
        const Token *t = advance();
        ASTNode *n = new_node(NODE_VAR);
        strncpy(n->name, t->value, MAX_IDENT_LEN - 1);
        return n;
    }
    if (match(TOK_LPAREN)) {
        ASTNode *e = parse_expr();
        if (!e) return NULL;
        if (!expect(TOK_RPAREN, "')'")) return NULL;
        return e;
    }
    fprintf(stderr, COL_RED "[Parser] Unexpected token '%s' at line %d\n" COL_RESET,
            peek()->value, peek()->line);
    return NULL;
}

/* ── Public API ──────────────────────────────────────────────────────── */
ASTNode *parser_parse(const Token *tokens, int token_count) {
    g_tokens = tokens;
    g_count  = token_count;
    g_pos    = 0;

    /* Parse top-level as a list of statements (implicit block) */
    ASTNode *root = new_node(NODE_BLOCK);
    int capacity = 32;
    root->stmts = malloc(capacity * sizeof(ASTNode*));
    root->stmt_count = 0;

    while (!check(TOK_EOF)) {
        ASTNode *s = parse_stmt();
        if (!s) { parser_free_ast(root); return NULL; }
        if (root->stmt_count == capacity) {
            capacity *= 2;
            root->stmts = realloc(root->stmts, capacity * sizeof(ASTNode*));
        }
        root->stmts[root->stmt_count++] = s;
    }
    return root;
}

/* ── AST printer ─────────────────────────────────────────────────────── */
static void indent_print(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

void parser_print_ast(const ASTNode *node, int indent) {
    if (!node) return;
    indent_print(indent);

    switch (node->type) {
        case NODE_NUM:
            printf(COL_YELLOW "NUM(%d)\n" COL_RESET, node->num_value);
            break;
        case NODE_VAR:
            printf(COL_CYAN "VAR(%s)\n" COL_RESET, node->name);
            break;
        case NODE_BINOP: {
            const char *opname = "?";
            switch (node->op) {
                case '+': opname = "ADD"; break; case '-': opname = "SUB"; break;
                case '*': opname = "MUL"; break; case '/': opname = "DIV"; break;
                case '<': opname = "LT";  break; case '>': opname = "GT";  break;
                case 'l': opname = "LE";  break; case 'g': opname = "GE";  break;
                case 'e': opname = "EQ";  break; case 'n': opname = "NEQ"; break;
            }
            printf(COL_GREEN "BINOP(%s)\n" COL_RESET, opname);
            parser_print_ast(node->left,  indent + 1);
            parser_print_ast(node->right, indent + 1);
            break;
        }
        case NODE_ASSIGN:
            printf(COL_MAGENTA "ASSIGN(%s)\n" COL_RESET, node->name);
            parser_print_ast(node->right, indent + 1);
            break;
        case NODE_PRINT:
            printf(COL_MAGENTA "PRINT\n" COL_RESET);
            parser_print_ast(node->right, indent + 1);
            break;
        case NODE_IF:
            printf(COL_BLUE "IF\n" COL_RESET);
            indent_print(indent + 1); printf("COND:\n");
            parser_print_ast(node->cond, indent + 2);
            indent_print(indent + 1); printf("THEN:\n");
            parser_print_ast(node->then_br, indent + 2);
            if (node->else_br) {
                indent_print(indent + 1); printf("ELSE:\n");
                parser_print_ast(node->else_br, indent + 2);
            }
            break;
        case NODE_WHILE:
            printf(COL_BLUE "WHILE\n" COL_RESET);
            indent_print(indent + 1); printf("COND:\n");
            parser_print_ast(node->cond, indent + 2);
            indent_print(indent + 1); printf("BODY:\n");
            parser_print_ast(node->then_br, indent + 2);
            break;
        case NODE_BLOCK:
            printf(COL_RESET "BLOCK (%d stmts)\n", node->stmt_count);
            for (int i = 0; i < node->stmt_count; i++)
                parser_print_ast(node->stmts[i], indent + 1);
            break;
    }
}

/* ── Free AST ────────────────────────────────────────────────────────── */
void parser_free_ast(ASTNode *node) {
    if (!node) return;
    parser_free_ast(node->left);
    parser_free_ast(node->right);
    parser_free_ast(node->cond);
    parser_free_ast(node->then_br);
    parser_free_ast(node->else_br);
    if (node->type == NODE_BLOCK) {
        for (int i = 0; i < node->stmt_count; i++)
            parser_free_ast(node->stmts[i]);
        free(node->stmts);
    }
    free(node);
}
