#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token type
typedef enum {
    TK_RESERVED, // symbol
    TK_IDENT, // indentifier
    TK_NUM, // integer
    TK_EOF, // EOF
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type; // token type
    Token *next; // next token
    int val; // the value of number
    char *str; // token string
    int len; // token length
};

// node type
typedef enum {
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
    ND_NUM, // integer
} NodeType;

typedef struct Node Node;
struct Node {
    NodeType type; // node type
    Node *next; // next node
    Node *lhs; // left-hand side
    Node *rhs; // right-hand side
    int val; // value (used if type == ND_NUM)
    char *name; // variable name (used if type == ND_LVAR)
};

extern Token *current_token;
extern char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool read(char *op);
Token *read_ident();
void expect(char *op);
int get_number();
bool at_eof();
Token *new_token(TokenType type, Token *cur, char *str, int len);
Token *tokenizer();

Node *new_node(NodeType type);
Node *new_binary(NodeType type, Node *lhs, Node *rhs);
Node *new_num(int val);
Node *new_lvar(char *name);

Node *program();

void gen_code(Node *node);