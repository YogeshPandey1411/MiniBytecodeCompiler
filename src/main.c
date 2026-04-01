#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

int main() {
    char input[] = "print x + 5";

    Token tokens[20];
    int count = tokenize(input, tokens);

    printf("TOKENS:\n");
    for (int i = 0; i < count; i++) {
        printf("%s ", tokens[i].value);
    }

    printf("\n\nBYTECODE:\n");

    ASTNode *tree = parse(tokens, count);
    generate(tree);

    return 0;
}