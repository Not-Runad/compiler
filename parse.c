#include "9cc.h"

VarList *var_list;

Node *new_node(NodePattern pattern) {
    Node *node = calloc(1, sizeof(Node));
    node->pattern = pattern;
    return node;
}

Node *new_binary(NodePattern pattern, Node *lhs, Node *rhs) {
    Node *node = new_node(pattern);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_val_node(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *new_var_node(Var *Var) {
    Node *node = new_node(ND_VAR);
    node->var = Var;
    return node;
}

Function *new_fn(char *fn_name, Node *node, VarList *params, VarList *var_list) {
    Function *fn = calloc(1, sizeof(Function));
    fn->name = fn_name;
    fn->node = node;
    fn->params = params;
    fn->var_list = var_list;
    return fn;
}

// append var to var_list
void *append_var(Var *var) {
    VarList *vl = calloc(1, sizeof(VarList));
    vl->var = var;
    vl->next = var_list;
    var_list = vl;
}

// get new var
Var *new_var(char *name) {
    Var *var = calloc(1, sizeof(Var));
    var->name = name;
    append_var(var);
    return var;
}

Var *find_var(Token *token) {
    for (VarList *vl = var_list; vl; vl = vl->next) {
        Var *var = vl->var;
        if (strlen(var->name) == token->len
            && !memcmp(token->str, var->name, token->len))
            return var;
    }
    return NULL;
}

VarList *read_fn_param() {
    // no param
    if (read_next_token(")"))
        return NULL;
    
    VarList *head = calloc(1, sizeof(VarList));
    head->var = new_var(get_ident());
    VarList *cur = head;
    while (!read_next_token(")")) {
        expect(",");
        cur->next = calloc(1, sizeof(VarList));
        cur->next->var = new_var(get_ident());
        cur = cur->next;
    }
    return head;
}

Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// program = function*
Function *program() {
    Function head;
    head.next = NULL;
    Function *cur = &head;

    while (!at_eof()) {
        cur->next = function();
        cur = cur->next;
    }
    return head.next;
}

// function = ident "(" params? ")" "{" stmt* "}"
// params = ident ("," ident)*
Function *function() {
    var_list = NULL;
    char *ident = get_ident();
    expect("(");
    VarList *params = read_fn_param();

    expect("{");

    Node head;
    head.next = NULL;
    Node *cur = &head;

    while (!read_next_token("}")) {
        cur->next = stmt();
        cur = cur->next;
    }

    Function *fn = new_fn(ident, head.next, params, var_list);
    return fn;
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
    if (read_next_token("return")) {
        node = new_binary(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    }

    // if-else statement
    if (read_next_token("if")) {
        Node *node = new_node(ND_IF);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (read_next_token("else"))
            node->els = stmt();
        return node;
    }

    // while statement
    if (read_next_token("while")) {
        Node *node = new_node(ND_WHILE);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }

    // for statement
    if (read_next_token("for")) {
        Node *node = new_node(ND_FOR);
        expect("(");

        // if initialize statement exists
        if (!read_next_token(";")) {
            node->init = expr();
            expect(";");
        }

        // if conditional statement exists
        if (!read_next_token(";")) {
            node->cond = expr();
            expect(";");
        }

        // if increment statement exists
        if (!read_next_token(")")) {
            node->inc = expr();
            expect(")");
        }

        node->then = stmt();
        return node;
    }

    // block
    if (read_next_token("{")) {
        Node head;
        head.next = NULL;
        Node *cur = &head;

        while (!read_next_token("}")) {
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
    if (read_next_token("="))
        return new_binary(ND_ASSIGN, node, assign());
    return node;
}

// equarity = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();

    while (1) {
        if (read_next_token("=="))
            node = new_binary(ND_EQ, node, relational());
        else if (read_next_token("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    while (1) {
        if (read_next_token("<"))
            node = new_binary(ND_LT, node, add());
        else if (read_next_token("<="))
            node = new_binary(ND_LE, node, add());
        else if (read_next_token(">"))
            node = new_binary(ND_LT, add(), node);
        else if (read_next_token(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    while (1) {
        if (read_next_token("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (read_next_token("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();
    
    while (1) {
        if (read_next_token("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (read_next_token("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-" | "&" | "*")? unary
//       | primary
Node *unary() {
    if (read_next_token("+"))
        return unary();
    if (read_next_token("-"))
        return new_binary(ND_SUB, new_val_node(0), unary());
    if (read_next_token("&"))
        return new_binary(ND_ADDR, unary(), NULL);
    if (read_next_token("*"))
        return new_binary(ND_DEREF, unary(), NULL);
    return primary();
}

// args = "(" (assign ("," assign)*)? ")"
Node *args() {
    // no args
    if (read_next_token(")"))
        return NULL;
    
    // parse args
    Node *head = assign();
    Node *cur = head;
    while (read_next_token(",")) {
        cur->next = assign();
        cur = cur->next;
    }

    expect(")");
    return head;
}

// primary = "(" expr ")"
//         | ident args?
//         | num
Node *primary() {
    // "(" expr ")"
    if (read_next_token("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // ident args?
    Token *ident_token = read_next_ident();
    if (ident_token) {
        // function
        if (read_next_token("(")) {
            Node *node = new_node(ND_FUNCALL);
            node->fn_name = strndup(ident_token->str, ident_token->len);
            node->args = args();
            return node;
        }
        
        // variable
        Var *var = find_var(ident_token);
        if (!var) { // not exist
            char *ident = strndup(ident_token->str, ident_token->len);
            var = new_var(ident);
            return new_var_node(var);
        }
        return new_var_node(var);
    }

    // parse num to value
    return new_val_node(get_number());
}