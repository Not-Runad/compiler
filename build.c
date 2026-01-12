#include "9cc.h"

int seq_label = 1;
char *arg_reg[] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9"
};

void gen_addr(Node *node) {
    if (node->type != ND_VAR)
        error("not an left value");
    
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    push rax\n");
}

void load() {
    printf("    pop rax\n");
    printf("    mov rax, [rax]\n");
    printf("    push rax\n");
}

void store() {
    printf("    pop rdi\n");
    printf("    pop rax\n");
    printf("    mov [rax], rdi\n");
    printf("    push rdi\n");
}

void gen_code(Node *node) {
    switch (node->type) {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_VAR:
        gen_addr(node);
        load();
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        gen_code(node->rhs);
        store();
        return;
    case ND_IF:
        if (node->els) {
            gen_code(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lelse%d\n", seq_label);
            gen_code(node->then);
            printf("    jmp .Lend%d\n", seq_label);
            printf(".Lelse%d:\n", seq_label);
            gen_code(node->els);
            printf(".Lend%d:\n", seq_label);
        } else {
            gen_code(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", seq_label);
            gen_code(node->then);
            printf(".Lend%d:\n", seq_label);
        }
        seq_label++;
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", seq_label);
        gen_code(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", seq_label);
        gen_code(node->then);
        printf("    jmp .Lbegin%d\n", seq_label);
        printf(".Lend%d:\n", seq_label);
        seq_label++;
        return;
    case ND_FOR:
        if (node->init)
            gen_code(node->init);
        printf(".Lbegin%d:\n", seq_label);
        if (node->cond) {
            gen_code(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", seq_label);
        }
        gen_code(node->then);
        if (node->inc)
            gen_code(node->inc);
        printf("    jmp .Lbegin%d\n", seq_label);
        printf(".Lend%d:\n", seq_label);
        return;
    case ND_RETURN:
        gen_code(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_BLOCK:
        for (Node *n = node->stmts; n; n = n->next)
            gen_code(n);
        return;
    case ND_FUNCALL:
        // generate values and count args
        int argc = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_code(arg);
            argc++;
        }

        // set values to registers by following x86-64 ABI
        for (int i = argc - 1; i >= 0; i--)
            printf("    pop %s\n", arg_reg[i]);

        printf("    call %s\n", node->func_name);
        printf("    push rax\n");
        return;
    }

    gen_code(node->lhs);
    gen_code(node->rhs);

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
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}

void build(Node *node) {
    // prologue
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // initialize
    // allocate variables memory
    int allocate_size = 0;
    for (Var *var = var_list; var; var = var->next)
        allocate_size += 8;
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", allocate_size);

    // generate code
    for (Node *n = node; n; n = n->next) {
        gen_code(n);
        // expr result is left, so pop it to prevent an stack-overflow
        printf("    pop rax\n");
    }

    // epilogue
    // last expr result in rax is return value
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}