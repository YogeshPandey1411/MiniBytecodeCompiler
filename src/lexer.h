#ifndef LEXER_H
#define LEXER_H

typedef enum {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    EQUAL
} TokenType;

typedef struct {
    TokenType type;
    char value[20];
} Token;

int tokenize(char *input, Token tokens[]);

#endif