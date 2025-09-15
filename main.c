#include "9cc.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid number of arguments.\n", argv[0]);
        return 1;
    }

    // tokenize and parse
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // print assembly's first half
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // generate code
    gen(node);

    // result value may be left at stack-top.
    // load it to RAX as return value from function.
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}