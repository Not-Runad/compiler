#include "9cc.h"

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
    codegen(node);

    // epilogue
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}