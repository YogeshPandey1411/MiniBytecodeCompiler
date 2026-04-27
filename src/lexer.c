#include "lexer.h"

/* ── Keyword table ───────────────────────────────────────────────────── */
static struct { const char *word; TokenType type; } keywords[] = {
    { "let",   TOK_LET   },
    { "print", TOK_PRINT },
    { "if",    TOK_IF    },
    { "else",  TOK_ELSE  },
    { "while", TOK_WHILE },
    { NULL,    TOK_UNKNOWN }
};

static TokenType keyword_or_ident(const char *s) {
    for (int i = 0; keywords[i].word; i++)
        if (strcmp(s, keywords[i].word) == 0)
            return keywords[i].type;
    return TOK_IDENT;
}

/* ── Main tokeniser ──────────────────────────────────────────────────── */
int lexer_tokenize(const char *src, Token *tokens, int max_tokens) {
    int  pos   = 0;
    int  count = 0;
    int  line  = 1;
    int  len   = (int)strlen(src);

    while (pos < len && count < max_tokens - 1) {
        /* Skip whitespace */
        if (src[pos] == '\n') { line++; pos++; continue; }
        if (isspace((unsigned char)src[pos])) { pos++; continue; }

        /* Single-line comment */
        if (src[pos] == '/' && src[pos+1] == '/') {
            while (pos < len && src[pos] != '\n') pos++;
            continue;
        }

        Token *t = &tokens[count];
        t->line  = line;

        /* Number literal */
        if (isdigit((unsigned char)src[pos])) {
            int start = pos;
            while (pos < len && isdigit((unsigned char)src[pos])) pos++;
            int sz = pos - start;
            strncpy(t->value, src + start, sz);
            t->value[sz] = '\0';
            t->type = TOK_NUMBER;
            count++;
            continue;
        }

        /* Identifier / keyword */
        if (isalpha((unsigned char)src[pos]) || src[pos] == '_') {
            int start = pos;
            while (pos < len && (isalnum((unsigned char)src[pos]) || src[pos] == '_')) pos++;
            int sz = pos - start;
            strncpy(t->value, src + start, sz);
            t->value[sz] = '\0';
            t->type = keyword_or_ident(t->value);
            count++;
            continue;
        }

        /* Multi-char operators */
        char c  = src[pos];
        char c2 = src[pos+1];

#define TOK2(a,b,tp) if (c==(a) && c2==(b)) { t->type=(tp); t->value[0]=(a); t->value[1]=(b); t->value[2]='\0'; count++; pos+=2; continue; }
        TOK2('=','=', TOK_EQEQ)
        TOK2('!','=', TOK_NEQ)
        TOK2('<','=', TOK_LE)
        TOK2('>','=', TOK_GE)
#undef TOK2

        /* Single-char tokens */
        t->value[0] = c; t->value[1] = '\0';
        switch (c) {
            case '+': t->type = TOK_PLUS;    break;
            case '-': t->type = TOK_MINUS;   break;
            case '*': t->type = TOK_STAR;    break;
            case '/': t->type = TOK_SLASH;   break;
            case '%': t->type = TOK_PERCENT; break;
            case '=': t->type = TOK_EQ;     break;
            case '<': t->type = TOK_LT;     break;
            case '>': t->type = TOK_GT;     break;
            case '(': t->type = TOK_LPAREN; break;
            case ')': t->type = TOK_RPAREN; break;
            case '{': t->type = TOK_LBRACE; break;
            case '}': t->type = TOK_RBRACE; break;
            case ';': t->type = TOK_SEMI;   break;
            default:
                fprintf(stderr, COL_RED "[Lexer] Unknown character '%c' at line %d\n" COL_RESET, c, line);
                return -1;
        }
        count++;
        pos++;
    }

    /* EOF sentinel */
    tokens[count].type    = TOK_EOF;
    tokens[count].value[0] = '\0';
    tokens[count].line     = line;
    count++;
    return count;
}

/* ── Pretty printer ──────────────────────────────────────────────────── */
const char *token_type_name(TokenType t) {
    switch (t) {
        case TOK_NUMBER:  return "NUMBER";
        case TOK_IDENT:   return "IDENT";
        case TOK_LET:     return "LET";
        case TOK_PRINT:   return "PRINT";
        case TOK_IF:      return "IF";
        case TOK_ELSE:    return "ELSE";
        case TOK_WHILE:   return "WHILE";
        case TOK_PLUS:    return "PLUS";
        case TOK_MINUS:   return "MINUS";
        case TOK_STAR:    return "STAR";
        case TOK_SLASH:   return "SLASH";
        case TOK_PERCENT: return "PERCENT";
        case TOK_EQ:      return "ASSIGN";
        case TOK_EQEQ:    return "EQ_EQ";
        case TOK_NEQ:     return "NOT_EQ";
        case TOK_LT:      return "LT";
        case TOK_GT:      return "GT";
        case TOK_LE:      return "LE";
        case TOK_GE:      return "GE";
        case TOK_LPAREN:  return "LPAREN";
        case TOK_RPAREN:  return "RPAREN";
        case TOK_LBRACE:  return "LBRACE";
        case TOK_RBRACE:  return "RBRACE";
        case TOK_SEMI:    return "SEMI";
        case TOK_EOF:     return "EOF";
        default:          return "UNKNOWN";
    }
}

void lexer_print_tokens(const Token *tokens, int count) {
    printf("\n  %-6s  %-12s  %s\n", "LINE", "TYPE", "VALUE");
    printf("  %-6s  %-12s  %s\n", "------", "------------", "-------------");
    for (int i = 0; i < count; i++) {
        const char *col = COL_RESET;
        switch (tokens[i].type) {
            case TOK_NUMBER:                              col = COL_YELLOW;  break;
            case TOK_IDENT:                               col = COL_CYAN;    break;
            case TOK_LET: case TOK_PRINT:
            case TOK_IF:  case TOK_ELSE: case TOK_WHILE: col = COL_MAGENTA; break;
            case TOK_EOF:                                 col = COL_RED;     break;
            default:                                      col = COL_GREEN;   break;
        }
        printf("  %-6d  %s%-12s%s  '%s'\n",
               tokens[i].line,
               col, token_type_name(tokens[i].type), COL_RESET,
               tokens[i].value);
    }
}
