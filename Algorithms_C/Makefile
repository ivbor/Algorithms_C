CC = gcc -Wall -Werror -Wextra -g
SHARED = -fPIC -shared
BIN = -fsanitize=address

tidy: 
	clang-tidy $(shell find . -not -path "./dp_solutions_from_page/*" -name '*.c' -o -name '*.h') -- -Iinclude -Isrc/headers -fno-caret-diagnostics -fdiagnostics-show-option

format:
	clang-format -Werror -i $(shell find . -name '*.c' -o -name '*.h')

launch: test/bin/all_tests
	test/bin/all_tests

clean:
	rm -f ./include/bin/*
	rm -f ./test/bin/*
	rm -f ./src/bin/*

test/bin/all_tests: \
		test/runners/all_tests.c \
		test/bin/test_array_count_sort.so
	$(CC) $(BIN) \
		test/bin/test_array_count_sort.so \
		test/runners/all_tests.c \
		-o test/bin/all_tests \
		-Wl,-rpath=./src/bin:./include/bin\
		-Lsrc/bin -l:matrix_view.so -l:array_count_sort.so -l:compare.so \
		-Linclude/bin -l:minunit.so -l:logger.so

test/bin/test_array_count_sort.so: \
		test/test/test_array_count_sort.c \
		include/bin/minunit.so \
		include/bin/logger.so \
		src/bin/matrix_view.so src/bin/compare.so \
		src/bin/array_count_sort.so
	$(CC) $(SHARED) -Isrc/headers -Iinclude \
		test/test/test_array_count_sort.c \
		src/bin/matrix_view.so src/bin/compare.so \
		src/bin/array_count_sort.so \
		-o test/bin/test_array_count_sort.so \
		-Wl,-rpath=./src/bin:./include/bin \
		-Linclude/bin -l:minunit.so -l:logger.so

src/bin/matrix_view.so: src/code/matrix_view.c
	$(CC) $(SHARED) -o src/bin/matrix_view.so src/code/matrix_view.c

src/bin/array_count_sort.so: src/code/array_count_sort.c
	$(CC) $(SHARED) -o src/bin/array_count_sort.so \
		src/code/array_count_sort.c

src/bin/compare.so: src/code/compare.c
	$(CC) $(SHARED) -o src/bin/compare.so src/code/compare.c

include/bin/minunit.so: include/minunit.c
	$(CC) $(SHARED) -o include/bin/minunit.so include/minunit.c

include/bin/logger.so: include/logger.c
	$(CC) $(SHARED) -o include/bin/logger.so include/logger.c