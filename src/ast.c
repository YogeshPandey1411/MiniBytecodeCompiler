#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* createNode(char *type, char *value) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    strcpy(node->type, type);
    strcpy(node->value, value);
    node->left = node->right = NULL;
    return node;
}