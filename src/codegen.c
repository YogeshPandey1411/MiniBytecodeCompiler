#include "codegen.h"
#include <stdio.h>
#include <string.h>


void generate(ASTNode *root) {
    if (strcmp(root->type, "ASSIGN") == 0) {
        printf("PUSH %s\n", root->right->value);
        printf("STORE %s\n", root->left->value);
    }

    else if (strcmp(root->type, "PRINT") == 0) {
        ASTNode *op = root->left;
        printf("LOAD %s\n", op->left->value);
        printf("PUSH %s\n", op->right->value);
        printf("ADD\n");
        printf("PRINT\n");
    }
}