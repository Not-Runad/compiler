#!/bin/bash

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

# assert 42 'return 42;'
# assert 21 'return 5+20-4;'
# assert 41 'return  12 + 34 - 5;'
# assert 47 'return 5 + 6 * 7;'
# assert 15 'return 5 * (9 - 6);'
# assert 4 'return (3 + 5) / 2;'
# assert 5 'return -3 * +5 + +20;'

# assert 0 'return 0 == 1;'
# assert 1 'return 42 == 42;'
# assert 1 'return 0 != 1;'
# assert 0 'return 42 != 42;'

# assert 1 'return 0 < 1;'
# assert 0 'return 1 < 1;'

# assert 1 'return 0 <= 1;'
# assert 1 'return 1 <= 1;'
# assert 0 'return 2 <= 1;'

# assert 0 'return 0 > 1;'
# assert 0 'return 1 > 1;'

# assert 0 'return 0 >= 1;'
# assert 1 'return 1 >= 1;'
# assert 1 'return 2 >= 1;'

# assert 0 'foo = bar = 3; return -foo + +bar;'

# assert 2 '1; return 2; 3;'

# assert 5 'a = 5; b = 10; if (a > b) return a - b; else return b - a;'
# assert 0 'a = 5; b = 10; c = 15; if (a + b > c) return a + c - b; else return c - (a + b);'

# assert 10 'i = 0; while (i < 10) i = i + 1; return i;'

# assert 55 'i = 0; j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j;'
# assert 5 'for (;;) return 5; return 10;'

# assert 55 'i = 0; j = 0; while (i <= 10) { j = i + j; i = i + 1; } return j;'
assert '13' 'foo = 1; return foo + foo();'
assert '36' 'bar = 2; return bar + bar();'
assert '59' 'baz = 3; return baz + baz();'

printf "\n\033[1;32m=== OK ===\033[0m"