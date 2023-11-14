all: yacc lex uccompiler

yacc: uccompiler.y
	yacc -dvtg -Wcounterexamples --report=all uccompiler.y

lex: uccompiler.l
	lex uccompiler.l

uccompiler: lex.yy.c y.tab.c ast.c
	cc lex.yy.c y.tab.c ast.c -o uccompiler -ll -g -Wall -Wextra

clean:
	rm -f lex.yy.c lexer *.zip y.tab.c y.tab.h y.output y.gv

zip:
	zip uccompiler.zip uccompiler.l uccompiler.y ast.c ast.h

run:
	make && clear && ./lexer -t < inp.c