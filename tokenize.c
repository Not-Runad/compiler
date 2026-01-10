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

bool startwith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || c == '_';
}

bool is_alnum(char c) {
    return is_alpha(c) || ('0' <= c && c <= '9');
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

        // multi-letter punctuator
        if (startwith(p, "==") 
            || startwith(p, "!=")
            || startwith(p, "<=")
            || startwith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // single-letter punctuator
        if (strchr("+-*/()<>;=", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // return keyword
        if (startwith(p, "return") && !is_alnum(p[6])) {
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
            continue;
        }

        // Identifier
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