#include "vm.h"

/* ── Variable store (name → int value) ──────────────────────────────── */
typedef struct {
    char name[MAX_IDENT_LEN];
    int  value;
} Variable;

static Variable vars[MAX_VARS];
static int      var_count;

static int var_get(const char *name, int *out) {
    for (int i = 0; i < var_count; i++)
        if (strcmp(vars[i].name, name) == 0) { *out = vars[i].value; return 1; }
    return 0;
}

static void var_set(const char *name, int value) {
    for (int i = 0; i < var_count; i++)
        if (strcmp(vars[i].name, name) == 0) { vars[i].value = value; return; }
    if (var_count >= MAX_VARS) {
        fprintf(stderr, COL_RED "[VM] Too many variables\n" COL_RESET);
        exit(1);
    }
    strncpy(vars[var_count].name, name, MAX_IDENT_LEN - 1);
    vars[var_count].value = value;
    var_count++;
}

/* ── Stack ───────────────────────────────────────────────────────────── */
static int stack[MAX_STACK];
static int sp = 0;   /* points to next free slot */

static void push(int v) {
    if (sp >= MAX_STACK) { fprintf(stderr, COL_RED "[VM] Stack overflow\n" COL_RESET); exit(1); }
    stack[sp++] = v;
}

static int pop(void) {
    if (sp <= 0) { fprintf(stderr, COL_RED "[VM] Stack underflow\n" COL_RESET); exit(1); }
    return stack[--sp];
}

/* ── Execute ─────────────────────────────────────────────────────────── */
int vm_execute(const CodeObject *obj) {
    /* Reset state */
    sp = 0;
    var_count = 0;

    int ip = 0;   /* instruction pointer */

    printf("\n  Program Output:\n");
    printf("  -----------------------------------------\n");

    while (ip < obj->code_len) {
        const Instruction *ins = &obj->code[ip];
        ip++;

        switch (ins->opcode) {

            case OP_PUSH:
                push(ins->operand);
                break;

            case OP_LOAD: {
                int val;
                if (!var_get(ins->var_name, &val)) {
                    fprintf(stderr, COL_RED "[VM] Undefined variable '%s'\n" COL_RESET, ins->var_name);
                    return -1;
                }
                push(val);
                break;
            }

            case OP_STORE: {
                int val = pop();
                var_set(ins->var_name, val);
                break;
            }

            case OP_ADD: { int b = pop(), a = pop(); push(a + b); break; }
            case OP_SUB: { int b = pop(), a = pop(); push(a - b); break; }
            case OP_MUL: { int b = pop(), a = pop(); push(a * b); break; }
            case OP_DIV: {
                int b = pop(), a = pop();
                if (b == 0) { fprintf(stderr, COL_RED "[VM] Division by zero\n" COL_RESET); return -1; }
                push(a / b);
                break;
            }

            case OP_CMP_EQ:  { int b = pop(), a = pop(); push(a == b); break; }
            case OP_CMP_NEQ: { int b = pop(), a = pop(); push(a != b); break; }
            case OP_CMP_LT:  { int b = pop(), a = pop(); push(a <  b); break; }
            case OP_CMP_GT:  { int b = pop(), a = pop(); push(a >  b); break; }
            case OP_CMP_LE:  { int b = pop(), a = pop(); push(a <= b); break; }
            case OP_CMP_GE:  { int b = pop(), a = pop(); push(a >= b); break; }

            case OP_PRINT:
                printf("  " COL_CYAN "%d\n" COL_RESET, pop());
                break;

            case OP_JMP:
                ip = ins->operand;
                break;

            case OP_JZ:
                if (pop() == 0) ip = ins->operand;
                break;

            case OP_HALT:
                printf("  -----------------------------------------\n");
                return 0;

            default:
                fprintf(stderr, COL_RED "[VM] Unknown opcode %d\n" COL_RESET, ins->opcode);
                return -1;
        }
    }
    return 0;
}
