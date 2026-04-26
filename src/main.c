#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

/* ── Built-in example programs ──────────────────────────────────────── */
static const char *EXAMPLE_PROGRAMS[] = {
    /* 0: arithmetic */
    "// Example 1: Arithmetic\n"
    "let a = 10;\n"
    "let b = 3;\n"
    "let sum = a + b;\n"
    "let product = a * b;\n"
    "print sum;\n"
    "print product;\n",

    /* 1: if-else */
    "// Example 2: If-Else\n"
    "let x = 15;\n"
    "if (x > 10) {\n"
    "    let result = x * 2;\n"
    "    print result;\n"
    "} else {\n"
    "    print x;\n"
    "}\n",

    /* 2: while loop - countdown */
    "// Example 3: While loop (countdown)\n"
    "let i = 5;\n"
    "while (i > 0) {\n"
    "    print i;\n"
    "    i = i - 1;\n"
    "}\n",

    /* 3: sum of first N numbers */
    "// Example 4: Sum 1..10\n"
    "let n = 10;\n"
    "let sum = 0;\n"
    "let i = 1;\n"
    "while (i <= n) {\n"
    "    sum = sum + i;\n"
    "    i = i + 1;\n"
    "}\n"
    "print sum;\n",

    NULL
};

static const char *EXAMPLE_NAMES[] = {
    "Arithmetic",
    "If-Else branching",
    "While loop (countdown)",
    "Sum of 1 to 10",
    NULL
};

/* ── Pipeline runner ─────────────────────────────────────────────────── */
static int run_pipeline(const char *source) {
    int ok = 1;

    /* ─── Phase 1: Lexer ─── */
    print_phase_banner("PHASE 1 : LEXER  (Tokeniser)", COL_CYAN);
    Token tokens[MAX_TOKENS];
    int token_count = lexer_tokenize(source, tokens, MAX_TOKENS);
    if (token_count < 0) {
        printf(COL_RED "  [!] Lexer failed.\n" COL_RESET);
        return 0;
    }
    lexer_print_tokens(tokens, token_count);
    printf(COL_GREEN "\n  [OK] Lexer produced %d token(s).\n" COL_RESET, token_count);

    /* ─── Phase 2: Parser ─── */
    print_phase_banner("PHASE 2 : PARSER  (AST Builder)", COL_YELLOW);
    ASTNode *ast = parser_parse(tokens, token_count);
    if (!ast) {
        printf(COL_RED "  [!] Parser failed.\n" COL_RESET);
        return 0;
    }
    parser_print_ast(ast, 1);
    printf(COL_GREEN "\n  [OK] Parser built AST successfully.\n" COL_RESET);

    /* ─── Phase 3: Code Generator ─── */
    print_phase_banner("PHASE 3 : CODE GENERATOR  (Bytecode)", COL_MAGENTA);
    CodeObject obj;
    if (codegen_generate(ast, &obj) < 0) {
        printf(COL_RED "  [!] Code generation failed.\n" COL_RESET);
        ok = 0;
    } else {
        codegen_print_code(&obj);
        printf(COL_GREEN "\n  [OK] Code generator emitted %d instruction(s).\n" COL_RESET, obj.code_len);
    }
    parser_free_ast(ast);
    if (!ok) return 0;

    /* ─── Phase 4: Virtual Machine ─── */
    print_phase_banner("PHASE 4 : VIRTUAL MACHINE  (Execution)", COL_GREEN);
    if (vm_execute(&obj) < 0) {
        printf(COL_RED "  [!] VM execution failed.\n" COL_RESET);
        return 0;
    }
    printf(COL_GREEN "  [OK] VM finished execution.\n" COL_RESET);
    return 1;
}

/* ── Read entire file into a buffer ─────────────────────────────────── */
static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, COL_RED "Cannot open file: %s\n" COL_RESET, path); return NULL; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

/* ── Interactive REPL-style input ────────────────────────────────────── */
static void run_interactive(void) {
    printf(COL_BOLD COL_CYAN
           "\n  Enter your program. Type '.' on a blank line when done.\n"
           COL_RESET);

    static char source[MAX_SOURCE_LEN];
    source[0] = '\0';
    char line[256];

    while (1) {
        printf(COL_YELLOW "  >>> " COL_RESET);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        if (strcmp(line, ".\n") == 0 || strcmp(line, ".") == 0) break;
        if (strlen(source) + strlen(line) < MAX_SOURCE_LEN - 1)
            strcat(source, line);
    }

    if (strlen(source) == 0) {
        printf(COL_RED "  No input provided.\n" COL_RESET);
        return;
    }

    printf("\n  -- Source --\n");
    printf("%s\n", source);
    run_pipeline(source);
}

/* ── Main menu ───────────────────────────────────────────────────────── */
static void print_banner(void) {
    printf("\n  Mini Bytecode Compiler & Virtual Machine\n");
    printf("  Compiler Design Project -- C99\n\n");
}

int main(int argc, char *argv[]) {
    print_banner();

    /* If a source file is passed as argument, just compile & run it */
    if (argc == 2) {
        char *src = read_file(argv[1]);
        if (!src) return 1;
        printf(COL_BOLD "\n  File: %s\n" COL_RESET, argv[1]);
        run_pipeline(src);
        free(src);
        return 0;
    }

    /* Interactive menu */
    while (1) {
        printf("\n  Choose an option:\n"
               "  1. Run a built-in example\n"
               "  2. Enter custom code\n"
               "  3. Exit\n");
        printf("  Choice: ");
        fflush(stdout);

        char choice[8];
        if (!fgets(choice, sizeof(choice), stdin)) break;
        int ch = atoi(choice);

        if (ch == 1) {
            printf(COL_BOLD "\n  Built-in examples:\n" COL_RESET);
            for (int i = 0; EXAMPLE_NAMES[i]; i++)
                printf("    %d. %s\n", i + 1, EXAMPLE_NAMES[i]);
            printf("  Choose example: ");
            fflush(stdout);
            if (!fgets(choice, sizeof(choice), stdin)) break;
            int ex = atoi(choice) - 1;
            if (ex < 0 || EXAMPLE_PROGRAMS[ex] == NULL) {
                printf(COL_RED "  Invalid choice.\n" COL_RESET);
                continue;
            }
            printf("\n  -- Source --\n");
            printf("%s\n", EXAMPLE_PROGRAMS[ex]);
            run_pipeline(EXAMPLE_PROGRAMS[ex]);

        } else if (ch == 2) {
            run_interactive();

        } else if (ch == 3) {
            printf(COL_GREEN "\n  Goodbye!\n\n" COL_RESET);
            break;

        } else {
            printf(COL_RED "  Unknown option.\n" COL_RESET);
        }
    }
    return 0;
}
