#include "codegen.h"

/* ── Codegen state ───────────────────────────────────────────────────── */
static CodeObject *g_obj;

/* ── Emit helpers ────────────────────────────────────────────────────── */
static int emit(Opcode op, int operand, const char *var_name) {
    if (g_obj->code_len >= MAX_CODE) {
        fprintf(stderr, COL_RED "[Codegen] Code buffer overflow\n" COL_RESET);
        return -1;
    }
    int idx = g_obj->code_len++;
    g_obj->code[idx].opcode  = op;
    g_obj->code[idx].operand = operand;
    if (var_name)
        strncpy(g_obj->code[idx].var_name, var_name, MAX_IDENT_LEN - 1);
    else
        g_obj->code[idx].var_name[0] = '\0';
    return idx;
}

/* Returns the index of the next instruction to be emitted */
static int current_addr(void) { return g_obj->code_len; }

/* Patch a jump target retroactively */
static void patch(int idx, int target) {
    g_obj->code[idx].operand = target;
}

/* ── Expression code generation ─────────────────────────────────────── */
static int gen_expr(const ASTNode *node) {
    if (!node) return -1;

    switch (node->type) {
        case NODE_NUM:
            emit(OP_PUSH, node->num_value, NULL);
            break;

        case NODE_VAR:
            emit(OP_LOAD, 0, node->name);
            break;

        case NODE_BINOP:
            if (gen_expr(node->left)  < 0) return -1;
            if (gen_expr(node->right) < 0) return -1;
            switch (node->op) {
                case '+': emit(OP_ADD,     0, NULL); break;
                case '-': emit(OP_SUB,     0, NULL); break;
                case '*': emit(OP_MUL,     0, NULL); break;
                case '/': emit(OP_DIV,     0, NULL); break;
                case '<': emit(OP_CMP_LT,  0, NULL); break;
                case '>': emit(OP_CMP_GT,  0, NULL); break;
                case 'l': emit(OP_CMP_LE,  0, NULL); break;
                case 'g': emit(OP_CMP_GE,  0, NULL); break;
                case 'e': emit(OP_CMP_EQ,  0, NULL); break;
                case 'n': emit(OP_CMP_NEQ, 0, NULL); break;
                default:
                    fprintf(stderr, COL_RED "[Codegen] Unknown operator\n" COL_RESET);
                    return -1;
            }
            break;

        default:
            fprintf(stderr, COL_RED "[Codegen] Non-expression node in expression\n" COL_RESET);
            return -1;
    }
    return 0;
}

/* ── Statement code generation ──────────────────────────────────────── */
static int gen_stmt(const ASTNode *node);

static int gen_block(const ASTNode *node) {
    for (int i = 0; i < node->stmt_count; i++)
        if (gen_stmt(node->stmts[i]) < 0) return -1;
    return 0;
}

static int gen_stmt(const ASTNode *node) {
    if (!node) return -1;

    switch (node->type) {

        case NODE_ASSIGN:
            if (gen_expr(node->right) < 0) return -1;
            emit(OP_STORE, 0, node->name);
            break;

        case NODE_PRINT:
            if (gen_expr(node->right) < 0) return -1;
            emit(OP_PRINT, 0, NULL);
            break;

        case NODE_IF: {
            /* Evaluate condition */
            if (gen_expr(node->cond) < 0) return -1;
            /* JZ → skip then-branch */
            int jz_idx = emit(OP_JZ, 0, NULL);

            /* then-branch */
            if (gen_block(node->then_br) < 0) return -1;

            if (node->else_br) {
                /* JMP → skip else-branch */
                int jmp_idx = emit(OP_JMP, 0, NULL);
                patch(jz_idx, current_addr());
                if (gen_block(node->else_br) < 0) return -1;
                patch(jmp_idx, current_addr());
            } else {
                patch(jz_idx, current_addr());
            }
            break;
        }

        case NODE_WHILE: {
            int loop_start = current_addr();
            if (gen_expr(node->cond) < 0) return -1;
            int jz_idx = emit(OP_JZ, 0, NULL);
            if (gen_block(node->then_br) < 0) return -1;
            emit(OP_JMP, loop_start, NULL);
            patch(jz_idx, current_addr());
            break;
        }

        case NODE_BLOCK:
            return gen_block(node);

        default:
            fprintf(stderr, COL_RED "[Codegen] Unexpected node type %d\n" COL_RESET, node->type);
            return -1;
    }
    return 0;
}

/* ── Public API ──────────────────────────────────────────────────────── */
int codegen_generate(const ASTNode *root, CodeObject *obj) {
    memset(obj, 0, sizeof(*obj));
    g_obj = obj;

    if (gen_block(root) < 0) return -1;
    emit(OP_HALT, 0, NULL);
    return 0;
}

/* ── Bytecode printer ────────────────────────────────────────────────── */
static const char *opcode_name(Opcode op) {
    switch (op) {
        case OP_PUSH:    return "PUSH";
        case OP_LOAD:    return "LOAD";
        case OP_STORE:   return "STORE";
        case OP_ADD:     return "ADD";
        case OP_SUB:     return "SUB";
        case OP_MUL:     return "MUL";
        case OP_DIV:     return "DIV";
        case OP_CMP_EQ:  return "CMP_EQ";
        case OP_CMP_NEQ: return "CMP_NEQ";
        case OP_CMP_LT:  return "CMP_LT";
        case OP_CMP_GT:  return "CMP_GT";
        case OP_CMP_LE:  return "CMP_LE";
        case OP_CMP_GE:  return "CMP_GE";
        case OP_PRINT:   return "PRINT";
        case OP_JMP:     return "JMP";
        case OP_JZ:      return "JZ";
        case OP_HALT:    return "HALT";
        default:         return "???";
    }
}

void codegen_print_code(const CodeObject *obj) {
    printf("\n  %-6s  %-10s  %s\n", "ADDR", "OPCODE", "OPERAND");
    printf("  %-6s  %-10s  %s\n", "------", "----------", "----------");
    for (int i = 0; i < obj->code_len; i++) {
        const Instruction *ins = &obj->code[i];
        printf("  " COL_YELLOW "%04d" COL_RESET "  " COL_GREEN "%-10s" COL_RESET "  ",
               i, opcode_name(ins->opcode));
        switch (ins->opcode) {
            case OP_PUSH:
                printf(COL_CYAN "%d" COL_RESET, ins->operand);
                break;
            case OP_LOAD:
            case OP_STORE:
                printf(COL_CYAN "%s" COL_RESET, ins->var_name);
                break;
            case OP_JMP:
            case OP_JZ:
                printf(COL_MAGENTA "-> %04d" COL_RESET, ins->operand);
                break;
            default:
                break;
        }
        printf("\n");
    }
}
