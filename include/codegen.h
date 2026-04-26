#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"

/* Walk the AST and emit bytecode into `obj`.
   Returns 0 on success, -1 on error. */
int  codegen_generate(const ASTNode *root, CodeObject *obj);

/* Pretty-print the bytecode listing (Phase 3 output). */
void codegen_print_code(const CodeObject *obj);

#endif
