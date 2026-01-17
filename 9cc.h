#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// local variable type
typedef struct Var Var;
struct Var {
    char *name; // variable name
    int offset; // offset from RBP
};

typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};


// token type
typedef enum {
    TK_RESERVED, // symbol
    TK_IDENT, // indentifier
    TK_NUM, // integer
    TK_EOF // EOF
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
    ND_ADDR, // address &
    ND_DEREF, // dereference *
    ND_VAR, // local variable
    ND_NUM, // integer
    ND_IF, // if
    ND_WHILE, // while
    ND_FOR, // for
    ND_RETURN, // return
    ND_BLOCK, // block { ... }
    ND_FUNCALL, // call function
} NodeType;

typedef struct Node Node;
struct Node {
    NodeType type; // node type
    Node *next; // next node
    Node *lhs; // left-hand side
    Node *rhs; // right-hand side

    // number value
    int val; // value (used if type == ND_NUM)

    // local variable
    Var *var; // variable (used if type == ND_VAR)
    
    // conditional statement
    Node *cond; // condition
    Node *then; // then
    Node *els; // else
    Node *init; // initializer
    Node *inc; // increment

    // in block
    Node *stmts; // statements in block

    // function
    char *fn_name; // function name
    Node *args; // function args
};

typedef struct Function Function;
struct Function {
    Function *next; // next function
    char *name; // function name
    VarList *params; // function params
    VarList *var_list; // varible list
    Node *node; // node in function
};


void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool read_next_token(char *op);
Token *read_next_ident();
void expect(char *op);
char *get_ident();
int get_number();
bool at_eof();

Function *program();

Token *tokenizer();
void build(Function *program);

// global variables
extern Token *current_token;
extern char *user_input;