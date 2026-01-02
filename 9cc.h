#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token type
typedef enum {
    TK_RESERVED, // symbol
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
    ND_NUM, // integer
} NodeType;

typedef struct Node Node;
struct Node {
    NodeType type; // node type
    Node *lhs; // left-hand side
    Node *rhs; // right-hand side
    int val; // value (when the type is ND_NUM)
};

extern Token *current_token;
extern char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool read(char *op);
void expect(char *op);
int get_number();
bool at_eof();
Token *new_token(TokenType type, Token *cur, char *str, int len);
Token *tokenizer();

Node *new_node(NodeType type);
Node *new_binary(NodeType type, Node *lhs, Node *rhs);
Node *new_num(int val);

Node *expr();