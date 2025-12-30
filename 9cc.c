#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================= prototype declaration BEGIN ========================= 
// token type
typedef enum {
    TK_RESERVED, // symbol
    TK_NUM, // integer
    TK_EOF, // EOF
} TokenType;

// node type
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // integer
} NodeType;

typedef struct Token Token;
typedef struct Node Node;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool read(char op);
void expect(char op);
int get_number();
bool at_eof();
Token *new_token(TokenType type, Token *cur, char *str);
Token *tokenizer();

Node *new_node(NodeType type);
Node *new_binary(NodeType type, Node *lhs, Node *rhs);
Node *new_num(int val);
Node *expr();
Node *mul();
Node *unary();
Node *primary();

Token *current_token;
char *user_input;
// ========================= prototype declaration END ========================= 

// ========================= token BEGIN ========================= 
struct Token {
    TokenType type; // token type
    Token *next; // next token
    int val; // the value of number
    char *str; // token string
};

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
    fprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// read next token
bool read(char op) {
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED || current_token->str[0] != op)
        return false;
    current_token = current_token->next;
    return true;
}

// expect next token
void expect(char op) {
    // if next token is not expected symbol
    if (current_token->type != TK_RESERVED || current_token->str[0] != op)
        error_at(current_token->str, "expected '%c'", op);
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

Token *new_token(TokenType type, Token *cur, char *str) {
    Token *token = calloc(1, sizeof(Token));
    token->type = type;
    token->str = str;
    cur->next = token;
    return token;
}

Token *tokenizer() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strchr("+-*/()", *p)) {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "invalid token");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}
// ========================= token END ========================= 



// ========================= node BEGIN ========================= 

struct Node {
    NodeType type; // node type
    Node *lhs; // left-hand side
    Node *rhs; // right-hand side
    int val; // value (when the type is ND_NUM)
};

Node *new_node(NodeType type) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

Node *new_binary(NodeType type, Node *lhs, Node *rhs) {
    Node *node = new_node(type);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// expr = mul ("+" mul | "-" mul)*
Node *expr() {
    Node *node = mul();

    while (1) {
        if (read('+'))
            node = new_binary(ND_ADD, node, mul());
        else if (read('-'))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();
    
    while (1) {
        if (read('*'))
            node = new_binary(ND_MUL, node, unary());
        else if (read('/'))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
Node *unary() {
    if (read('+'))
        return primary();
    if (read('-'))
        return new_binary(ND_SUB, new_num(0), primary());
    return primary();
}

// primary = num | "(" expr ")"
Node *primary() {
    if (read('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_num(get_number());
}

void assembler(Node *node) {
    if (node->type == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    assembler(node->lhs);
    assembler(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->type) {
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
// ========================= node END ========================= 

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments");
        return 1;
    }

    // prologue
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // get user input
    user_input = argv[1];
    // tokenize
    current_token = tokenizer();
    // initialize expression
    Node *node = expr();

    // code generation
    assembler(node);

    // epilogue
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}