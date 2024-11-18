# Compiler and flags
CC = gcc
CFLAGS = -I../../inc -Wall -Wextra -g -O3 -pedantic -fsanitize=address

# Directories
SRC_DIR = ../../src
INC_DIR = ../../inc

# Source files
SRCS = ../../src/semantic_analysis.c \
       ../../src/ast.c \
       ../../src/symtable.c \
       ../../src/stack.c \
       ../../src/error.c


# Test file
TEST_FILE = test_semantic.c

# Output binary
TEST_BIN = semantic_test

# Default target
all: $(TEST_BIN)

# Build the test executable
$(TEST_BIN): $(TEST_FILE) $(SRCS)
	$(CC) $(CFLAGS) $(TEST_FILE) $(SRCS) -o $(TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(TEST_BIN)
