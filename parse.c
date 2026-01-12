#include "9cc.h"

Var *var_list;

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

Node *new_val(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *new_var(Var *Var) {
    Node *node = new_node(ND_VAR);
    node->var = Var;
    return node;
}

Var *find_lvar(Token *token) {
    for (Var *Var = var_list; Var; Var = Var->next) {
        if (strlen(Var->name) == token->len
            && !memcmp(token->str, Var->name, token->len))
            return Var;
    }
    return NULL;
}

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

// stmt =  expr ";"
//      | "{" stmt* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "return" expr ";"
Node *stmt() {
    Node *node;

    // return statement
    if (read("return")) {
        node = new_binary(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    }

    // if-else statement
    if (read("if")) {
        Node *node = new_node(ND_IF);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (read("else"))
            node->els = stmt();
        return node;
    }

    // while statement
    if (read("while")) {
        Node *node = new_node(ND_WHILE);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }

    // for statement
    if (read("for")) {
        Node *node = new_node(ND_FOR);
        expect("(");

        // if initialize statement exists
        if (!read(";")) {
            node->init = expr();
            expect(";");
        }

        // if conditional statement exists
        if (!read(";")) {
            node->cond = expr();
            expect(";");
        }

        // if increment statement exists
        if (!read(")")) {
            node->inc = expr();
            expect(")");
        }

        node->then = stmt();
        return node;
    }

    // block
    if (read("{")) {
        Node head;
        head.next = NULL;
        Node *cur = &head;

        while (!read("}")) {
            cur->next = stmt();
            cur = cur->next;
        }
        
        Node *node = new_node(ND_BLOCK);
        node->stmts = head.next;
        return node;
    }

    node = expr();
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
        return unary();
    if (read("-"))
        return new_binary(ND_SUB, new_val(0), primary());
    return primary();
}

// primary = "(" expr ")"
//         | ident ("(" ")")?
//         | num
Node *primary() {
    // "(" expr ")"
    if (read("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // ident ("(" ")")?
    Token *ident_token = read_ident();
    if (ident_token) {
        // if identifier is a function
        if (read("(")) {
            expect(")");
            Node *node = new_node(ND_FUNCALL);
            node->func_name = strndup(ident_token->str, ident_token->len);
            return node;
        }
        
        Var *var = find_lvar(ident_token);
        if (!var) { // not exist
            var = calloc(1, sizeof(Var));
            var->next = var_list;
            var->name = strndup(ident_token->str, ident_token->len);
            // whether var_list == NULL or not
            var->offset = var_list ? var_list->offset + 8 : 8;
            var_list = var;
        }
        return new_var(var);
    }

    // parse num to value
    return new_val(get_number());
}