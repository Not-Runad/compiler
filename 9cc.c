#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TK_RESERVED, // symbol
    TK_NUM, // integer
    TK_EOF, // EOF
} TokenType;

typedef struct Token Token;
struct Token
{
    TokenType type; // token type
    Token *next; // next token
    int val; // the value of number
    char *str; // token string
};

Token *current_token;

// report error
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// read next token
bool read(char op)
{
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED || current_token->str[0] != op)
        return false;
    current_token = current_token->next;
    return true;
}

// expect next token
void expect(char op)
{
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED || current_token->str[0] != op)
        error("Not '%c'", op);
    current_token = current_token->next;
}

// get number
int get_number()
{
    if (current_token->type != TK_NUM)
        error("Not number");
    int val = current_token->val;
    current_token = current_token->next;
    return val;
}

bool at_eof()
{
    return current_token->type == TK_EOF;
}

Token *new_token(TokenType type, Token *cur, char *str)
{
    Token *token = calloc(1, sizeof(Token));
    token->type = type;
    token->str = str;
    cur->next = token;
    return token;
}

Token *tokenizer(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-')
        {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // prologue
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // tokenize
    current_token = tokenizer(argv[1]);

    // check first number
    printf("    mov rax, %d\n", get_number());

    while (!at_eof())
    {
        if (read('+'))
        {
            printf("    add rax, %d\n", get_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", get_number());
    }

    printf("    ret\n");
    return 0;
}