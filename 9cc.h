#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local variable type
typedef struct Var Var;
struct Var {
    char *name; // Variable name
    int offset; // Offset from RBP
};

typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};

// Token pattern
typedef enum {
    TK_RESERVED, // Symbol
    TK_IDENT, // Indentifier
    TK_NUM, // Integer
    TK_EOF // EOF
} TokenPattern;

typedef struct Token Token;
struct Token {
    TokenPattern pattern; // Token pattern
    Token *next; // Next token
    int val; // Value of number
    char *str; // Token string
    int len; // Token length
};

// Node pattern
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
    ND_ADDR, // Address &
    ND_DEREF, // Dereference *
    ND_VAR, // Local variable
    ND_NUM, // Integer
    ND_IF, // if
    ND_WHILE, // while
    ND_FOR, // for
    ND_RETURN, // return
    ND_BLOCK, // Block { ... }
    ND_FUNCALL, // Call function
} NodePattern;

typedef struct Node Node;
struct Node {
    NodePattern pattern; // Node pattern
    Node *next; // Next node
    Node *lhs; // Left-hand side
    Node *rhs; // Right-hand side

    // Number value
    int val; // Value (used if type == ND_NUM)

    // Local variable
    Var *var; // Variable (used if type == ND_VAR)
    
    // conditional statement
    Node *cond; // Condition
    Node *then; // Then
    Node *els; // Else
    Node *init; // Initializer
    Node *inc; // Increment

    // In block
    Node *stmts; // Statements in block

    // Function
    char *fn_name; // Function name
    Node *args; // Function args
};

typedef struct Function Function;
struct Function {
    Function *next; // Next function
    char *name; // Function name
    VarList *params; // Function params
    VarList *var_list; // Varible list
    Node *node; // Node in function
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

// Global variables
extern Token *current_token;
extern char *user_input;