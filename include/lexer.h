#ifndef LEXER_H
#define LEXER_H

#include "common.h"

/* Tokenise `source` and fill `tokens[]`.
   Returns the number of tokens produced, or -1 on error. */
int  lexer_tokenize(const char *source, Token *tokens, int max_tokens);

/* Pretty-print the token list (Phase 1 output). */
void lexer_print_tokens(const Token *tokens, int count);

/* Human-readable name for a token type (for debugging). */
const char *token_type_name(TokenType t);

#endif
