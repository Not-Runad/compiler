#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TK_RESERVED, // symbol
    TK_IDENT, // identifier
    TK_NUM, // integer
    TK_EOF, // end of input
    TK_RETURN, // return
} TokenKind;

// token type
typedef struct Token Token;
struct Token
{
    TokenKind kind; // token type
    Token *next; // next input token
    int val; // the number of TK_NUM (if 'kind' is 'TK_NUM')
    char *str; // token string
    int len; // token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Token *consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
int is_alnum(char c);
bool startwith(char *p, char *q);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_LVAR, // local variable
    ND_NUM, // Integer
    ND_RETURN, // return
} NodeKind;

typedef struct Node Node;
struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val; // only used if kind == ND_NUM
    int offset; // only used if kind == ND_LVAR
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

typedef struct LVar LVar;

struct LVar
{
    LVar *next; // next var or NULL
    char *name; // var name
    int len; // name length
    int offset; // offset from RBP
};

LVar *find_lvar(Token *tok);

void gen_lval(Node *node);
void gen(Node *node);

extern char *user_input;
extern Token *token;
extern Node *code[100];
extern LVar *locals;

int main(int argc, char **argv);