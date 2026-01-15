#!/bin/bash

# generete tmp_func.o
gcc -xc -c -o tmp_func.o - <<EOF
int foo() { return 12; }
int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }

int add_6args(int a, int b, int c, int d, int e, int f) {
	return a + b + c + d + e + f;
}
EOF

# assertion
assert() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	gcc -static -o tmp tmp.s tmp_func.o
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		printf "$input => \033[1;32m$actual\033[0m\n"
	else
		printf "$input => \033[1;32m$expected\033[0m expected, but got \033[1;31m$actual\033[0m\n"
		exit 1
	fi
}

# four arithmetic operation
assert 42 'main() { return 42; }'
assert 41 'main() { return  12 + 34 - 5; }'
assert 15 'main() { return 5 * (9 - 6); }'
assert 4 'main() { return (3 + 5) / 2; }'
assert 5 'main() { return -3 * +5 + +20; }'

# condition
assert 0 'main() { return 0 == 1; }'
assert 1 'main() { return 42 == 42; }'
assert 1 'main() { return 0 != 1; }'
assert 0 'main() { return 42 != 42; }'

assert 1 'main() { return 0 < 1; }'
assert 0 'main() { return 1 < 1; }'

assert 1 'main() { return 0 <= 1; }'
assert 1 'main() { return 1 <= 1; }'
assert 0 'main() { return 2 <= 1; }'

assert 0 'main() { return 0 > 1; }'
assert 0 'main() { return 1 > 1; }'

assert 0 'main() { return 0 >= 1; }'
assert 1 'main() { return 1 >= 1; }'
assert 1 'main() { return 2 >= 1; }'

# identifier
assert 0 'main() { foo = bar = 3; return -foo + +bar; }'

# return
assert 2 'main() { 1; return 2; 3; }'

# if-else
assert 5 'main() { a = 5; b = 10; if (a > b) return a - b; else return b - a; }'
assert 0 'main() { a = 5; b = 10; c = 15; if (a + b > c) return a + c - b; else return c - (a + b); }'

# while
assert 10 'main() { i = 0; while (i < 10) i = i + 1; return i; }'

# for
assert 55 'main() { i = 0; j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j; }'
assert 5 'main() { for (;;) return 5; return 10; }'

# block
assert 55 'main() { i = 0; j = 0; while (i <= 10) { j = i + j; i = i + 1; } return j; }'

# zero-arity function
assert 13 'main() { foo = 1; return foo + foo(); }'

# multi-arity function with up to 6 args
assert 4 'main() { foo = 1; bar = 2; return add(foo + bar, bar - foo); }'
assert 2 'main() { foo = 1; bar = 2; return sub(foo + bar, bar - foo); }'
assert 21 'main() { return add_6args(1, 2, 3, 4, 5, 6); }'

# zero-arity function declare
assert 13 'main() { ret12 = 1; return ret12 + ret12(); } ret12() { return 12; }'

# multi-arity function declare
assert 10 'main() { return myadd(3, 7); } myadd(a, b) { return a + b; }'
assert 4 'main() { return mysub(7, 3); } mysub(a, b) { return a - b; }'
assert 55 'main() { return fib(9); } fib(x) { if (x <= 1) return 1; return fib(x - 1) + fib(x - 2); }'

# all correct
printf "\n\033[1;32m=== OK ===\033[0m\n"