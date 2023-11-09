# Makefile for uccompiler

# Specify the compiler
CC = cc
LEX = lex
YACC = yacc

# Define the source file and output executable
LEX_SOURCE = uccompiler.l
LEX_OUTPUT = lex.yy.c
YACC_SOURCE = uccompiler.y
YACC_OUTPUT = y.tab.c
OUTPUT = uccompiler

# Default target
all: $(OUTPUT)

# Generate y.tab.c from uccompiler.y
$(YACC_OUTPUT): $(YACC_SOURCE)
	$(YACC) -dv $(YACC_SOURCE)

# Generate lex.yy.c from uccompiler.l
$(LEX_OUTPUT): $(LEX_SOURCE)
	$(LEX) $(LEX_SOURCE)

# Compile the uccompiler executable
$(OUTPUT): $(YACC_OUTPUT) $(LEX_OUTPUT)
	$(CC) $(YACC_OUTPUT) $(LEX_OUTPUT) -o $(OUTPUT)

