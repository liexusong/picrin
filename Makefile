all: build run

build:
	cd src; \
	yacc -d parse.y; \
	lex scan.l
	gcc -o bin/picrin -I./include src/main.c src/state.c src/gc.c src/pair.c src/port.c src/symbol.c src/value.c src/y.tab.c src/lex.yy.c src/bool.c src/vm.c src/init.c

clean:
	rm -f src/y.tab.c src/y.tab.h src/lex.yy.c
	rm -f bin/picrin

run:
	bin/picrin
