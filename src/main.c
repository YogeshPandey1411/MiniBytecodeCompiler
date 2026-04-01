#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./compiler <input_file>\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    char input[100];
    fgets(input, sizeof(input), fp);
    fclose(fp);

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