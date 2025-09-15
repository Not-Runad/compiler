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

// input program
char *user_input;

// current token
Token *token;

// function of reporting error
// get same args as printf
void error(char *fmt, ...);

// report error place
void error_at(char *loc, char *fmt, ...);

// if next token is the expected symbol, it reads one token and return true.
// otherwise, it returns false.
bool consume(char *op);

// if next token is the expected symbol, it reads one token.
// otherwise, it reports error.
void expect(char *op);


// if next token is the expected symbol, it reads one token and return the value.
// otherwise, it reports error.
int expect_number();

bool at_eof();

// create token and connect it ot 'cur'
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

bool startwith(char *p, char *q);

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
    ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;
struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_num(int val);

Node *expr(); // expr = equality
Node *equality(); // equality = relational ("==" relational | "!=" relational)*
Node *relational(); // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add(); // add = mul ("+" mul | "-" mul)*
Node *mul(); // mul = unary ("*" unary | "/" unary)*
Node *unary(); // unary = ("+" | "-")? primary
Node *primary(); // primary = num | "(" expr ")"

void gen(Node *node);

int main(int argc, char **argv);
