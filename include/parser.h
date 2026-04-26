#ifndef PARSER_H
#define PARSER_H

#include "common.h"

/* Parse the token stream into an AST.
   Returns the root node (a BLOCK of statements), or NULL on error. */
ASTNode *parser_parse(const Token *tokens, int token_count);

/* Pretty-print the AST (Phase 2 output). */
void parser_print_ast(const ASTNode *node, int indent);

/* Free all AST nodes. */
void parser_free_ast(ASTNode *node);

#endif
