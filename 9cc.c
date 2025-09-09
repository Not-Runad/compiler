#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================
// token
// ================================================================
// kind of token
typedef enum
{
    TK_RESERVED, // symbol
    TK_NUM, // integer token
    TK_EOF, // end of input token
} TokenKind;

typedef struct Token Token;

// token type
typedef struct Token
{
    TokenKind kind; // token type
    Token *next; // next input token
    int val; // the number of TK_NUM (if 'kind' is 'TK_NUM')
    char *str; // token string
} Token;

// input program
char *user_input;

// current token
Token *token;

// function of reporting error
// get same args as printf
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// report error place
void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if next token is the expected symbol, it reads one token and return true.
// otherwise, it returns false.
bool consume(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// if next token is the expected symbol, it reads one token.
// otherwise, it reports error.
void expect(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "expeted '%c'", op);
    token = token->next;
}


// if next token is the expected symbol, it reads one token and return the value.
// otherwise, it reports error.
int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "expeted a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

// create token and connect it ot 'cur'
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize()
{
    char *p = user_input;
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

        if (strchr("+-*/()", *p))
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

        error("invalid token");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

typedef enum
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *primary();

Node *expr()
{
    Node *node = mul();
    while (1)
    {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume ('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul()
{
    Node *node = primary();
    while (1)
    {
        if (consume('*'))
            node = new_node(ND_MUL, node, primary());
        else if (consume('/'))
            node = new_node(ND_DIV, node, primary());
        else
            return node;
    }
}

Node *primary()
{
    if (consume('('))
    {
        Node *node = expr();
        expect(')');
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
    }
    
    printf("    push rax\n");
}

// =========================================================
// main
// =========================================================

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid number of arguments.\n", argv[0]);
        return 1;
    }

    // tokenize and parse
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // print assembly's first half
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // generate code
    gen(node);

    // result value may be left at stack-top.
    // load it to RAX as return value from function.
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}