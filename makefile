# Makefile for uccompiler

# Specify the compiler and flags
CC = cc
LEX = lex

# Define the source file and output executable
LEX_SOURCE = uccompiler.l
LEX_OUTPUT = lex.yy.c
OUTPUT = uccompiler

# Default target
all: $(OUTPUT)

# Generate lex.yy.c from uccompiler.l
$(LEX_OUTPUT): $(LEX_SOURCE)
	$(LEX) $(LEX_SOURCE)

# Compile the uccompiler executable
$(OUTPUT): $(LEX_OUTPUT)
	$(CC) $(LEX_OUTPUT) -o $(OUTPUT)

# Clean generated files and the executable
clean:
	rm -f $(LEX_OUTPUT) $(OUTPUT)

# Phony target to avoid conflicts with files named 'clean' or 'all'
.PHONY: clean all

