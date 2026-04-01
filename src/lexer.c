#include "lexer.h"
#include <stdio.h>
#include <string.h>


int tokenize(char *input, Token tokens[]) {
    int count = 0;
    char *word = strtok(input, " \n");

    while (word != NULL) {
        if (strcmp(word, "let") == 0 || strcmp(word, "print") == 0) {
            tokens[count].type = KEYWORD;
        } else if (strcmp(word, "=") == 0) {
            tokens[count].type = EQUAL;
        } else if (strcmp(word, "+") == 0 || strcmp(word, "-") == 0 ||
                   strcmp(word, "*") == 0 || strcmp(word, "/") == 0) {
            tokens[count].type = OPERATOR;
        } else if (word[0] >= '0' && word[0] <= '9') {
            tokens[count].type = NUMBER;
        } else {
            tokens[count].type = IDENTIFIER;
        }

        strcpy(tokens[count].value, word);
        count++;
        word = strtok(NULL, " \n");
    }

    return count;
}