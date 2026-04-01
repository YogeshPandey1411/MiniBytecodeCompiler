#include "parser.h"
#include <string.h>


ASTNode* parse(Token tokens[], int count) {
    if (strcmp(tokens[0].value, "let") == 0) {
        ASTNode *assign = createNode("ASSIGN", "=");
        assign->left = createNode("IDENTIFIER", tokens[1].value);
        assign->right = createNode("NUMBER", tokens[3].value);
        return assign;
    }

    if (strcmp(tokens[0].value, "print") == 0) {
        ASTNode *printNode = createNode("PRINT", "print");

        ASTNode *op = createNode("OPERATOR", tokens[2].value);
        op->left = createNode("IDENTIFIER", tokens[1].value);
        op->right = createNode("NUMBER", tokens[3].value);

        printNode->left = op;
        return printNode;
    }

    return NULL;
}