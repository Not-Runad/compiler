#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments");
        return 1;
    }

    // tokenize and parse
    user_input = argv[1];
    current_token = tokenizer();

    // build assembly
    build(program());

    return 0;
}