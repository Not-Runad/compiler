#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments");
        return 1;
    }

    // get user input
    user_input = argv[1];
    // tokenize
    current_token = tokenizer();
    // initialize expression
    Node *node = program();

    // prologue
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // allocate 26 variables memory
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    // code generation
    for (Node *n = node; n; n = n->next) {
        gen_code(n);
        // a value is left as expr result,
        // so pop it to prevent an stack-overflow
        printf("    pop rax\n");
    }

    // epilogue
    // last expr result on rax is return value
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}