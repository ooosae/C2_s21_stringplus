CC=gcc
CFLAGS=-Wall -pedantic -Wextra -Wfloat-equal -Wfloat-conversion -Wvla -std=c11 
COVFLAGS=-fprofile-arcs	-ftest-coverage

OS=$(shell uname)
ifeq ($(OS), Darwin)
	PKGFLAGS=-lcheck
else
	CHECKFLAGS=-lcheck -lpthread -lpthread -lrt -lm -lsubunit
endif

all: clean gcov_report

test:
	checkmk my.check > s21_string_test.c
	$(CC) s21_string_test.c s21_string.c s21_sscanf.c s21_sprintf.c $(PKGFLAGS) -o s21_string_test $(CHECKFLAGS) --coverage
	./s21_string_test

s21_string.a: clean
	$(CC)	$(CFLAGS) -c s21_string.c s21_sprintf.c s21_sscanf.c
	ar rc s21_string.a *.o
	ranlib s21_string.a

gcov_report: clean s21_string.a test
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory report
	rm -rf *.g* gcov_rep* coverage.info
	open report/index.html

clean:
	rm -rf *.a *.o report/ s21_string_test* *.out *.g* coverage.info

