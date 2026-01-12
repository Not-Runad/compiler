#include "9cc.h"

Token *current_token;
char *user_input;

// report error
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// report error position
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// read next token
bool read(char *op) {
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED
        || strlen(op) != current_token->len
        || memcmp(current_token->str, op, current_token->len))
        return false;
    current_token = current_token->next;
    return true;
}

Token *read_ident() {
    if (current_token->type != TK_IDENT)
        return NULL;
    Token *before_token = current_token;
    current_token = current_token->next;
    return before_token;
}

// expect next token
void expect(char *op) {
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED
        || strlen(op) != current_token->len
        || memcmp(current_token->str, op, current_token->len))
        error_at(current_token->str, "expected \"%s\"", op);
    current_token = current_token->next;
}

// get number
int get_number() {
    if (current_token->type != TK_NUM)
        error_at(current_token->str, "expected a number");
    int val = current_token->val;
    current_token = current_token->next;
    return val;
}

bool at_eof() {
    return current_token->type == TK_EOF;
}

bool is_alpha(char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || c == '_';
}

bool is_alnum(char c) {
    return is_alpha(c) || ('0' <= c && c <= '9');
}

bool startwith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

char *startwith_reserved(char *p) {
    // keyword
    static char *kw[] = {
        "if",
        "else",
        "while",
        "for",
        "return"
    };

    // decide match keyword
    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
        int len = strlen(kw[i]);
        if (startwith(p, kw[i]) && !is_alnum(p[len]))
            return kw[i];
    }

    // multi-letter punctuator
    static char *op[] = {
        "==",
        "!=",
        "<=",
        ">="
    };

    // decide matchj multi-letter punctuator
    for (int i = 0; i < sizeof(op) / sizeof(*op); i++) {
        if (startwith(p, op[i]))
            return op[i];
    }

    // unmatched
    return NULL;
}

Token *new_token(TokenType type, Token *cur, char *str, int len) {
    Token *token = calloc(1, sizeof(Token));
    token->type = type;
    token->str = str;
    token->len = len;
    cur->next = token;
    return token;
}

Token *tokenizer() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip whitespace chars
        if (isspace(*p)) {
            p++;
            continue;
        }

        // keyword or multi-letter punctuator
        char *kw = startwith_reserved(p);
        if (kw) {
            int len = strlen(kw);
            cur = new_token(TK_RESERVED, cur, p, len);
            p += len;
            continue;
        }

        // single-letter punctuator
        if (strchr("+-*/()<>;={},", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // identifier
        if (is_alpha(*p)) {
            char *q = p++;
            while (is_alnum(*p)) p++;
            cur = new_token(TK_IDENT, cur, q, p - q);
            continue;
        }

        // integer literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "invalid token");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}