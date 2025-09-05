#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of token
typedef enum {
    TK_RESERVED, // symbol
    TK_NUM, // integer token
    TK_EOF, // end of input token
} TokenKind;

typedef struct Token Token;

// token type
struct Token {
    TokenKind kind; // token type
    Token *next; // next input token
    int val; // the number of TK_NUM (if 'kind' is 'TK_NUM')
    char *str; // token string
};

// highlighting token
Token *token;

// function of reporting error
// get same args as printf
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if next token is the expected symbol, it reads one token and return true.
// otherwise, it returns false.
bool consume(char op) {
    if (token -> kind != TK_RESERVED || token -> str[0] != op) return false;
    token = token -> next;
    return true;
}

// if next token is the expected symbol, it reads one token.
// otherwise, it reports error.
void expect(char op) {
    if (token -> kind != TK_RESERVED || token -> str[0] != op) error("'%c' is wrong.", op);
    token = token -> next;
}


// if next token is the expected symbol, it reads one token and return the value.
// otherwise, it reports error.
int expect_number() {
    if (token -> kind != TK_NUM) error("it is not number.");
    int val = token -> val;
    token = token -> next;
    return val;
}

bool at_eof() {
    return token -> kind == TK_EOF;
}

// create token and connect it ot 'cur'
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok -> kind = kind;
    tok -> str = str;
    cur -> next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur -> val = strtol(p, &p, 10);
            continue;
        }

        error("Could not tokenize.");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        return 1;
    }

    // tokenize
    token = tokenize(argv[1]);

    // print assembly's first half
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // check the first token whether it is number or not,
    // and print first mov operation.
    printf("    mov rax, %d\n", expect_number());
    
    // consume tokens of '+ <num>' or '- <num>', and print assembly.
    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number);
            continue;
        }
        expect('-');
        printf("    sub rax, %d\n", expect_number);
    }
    
    printf("    ret\n");
    return 0;
}