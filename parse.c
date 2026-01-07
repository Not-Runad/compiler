#include "9cc.h"

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

Node *new_lvar(char *name) {
    Node *node = new_node(ND_LVAR);
    node->name = name;
    return node;
}

Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// program = stmt*
Node *program() {
    Node head;
    head.next = NULL;
    Node *cur = &head;

    while (!at_eof()) {
        cur->next = stmt();
        cur = cur->next;
    }
    return head.next;
}

// stmt = expr ";"
Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
Node *expr() {
    return assign();
}

// assign = equarity ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (read("="))
        return new_binary(ND_ASSIGN, node, assign());
    return node;
}

// equarity = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();

    while (1) {
        if (read("=="))
            node = new_binary(ND_EQ, node, relational());
        else if (read("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    while (1) {
        if (read("<"))
            node = new_binary(ND_LT, node, add());
        else if (read("<="))
            node = new_binary(ND_LE, node, add());
        else if (read(">"))
            node = new_binary(ND_LT, add(), node);
        else if (read(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    while (1) {
        if (read("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (read("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();
    
    while (1) {
        if (read("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (read("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
Node *unary() {
    if (read("+"))
        return primary();
    if (read("-"))
        return new_binary(ND_SUB, new_num(0), primary());
    return primary();
}

// primary = "(" expr ")" | ident | num
Node *primary() {
    if (read("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *ident_token = read_ident();
    if (ident_token)
        return new_lvar(ident_token->str);

    return new_num(get_number());
}