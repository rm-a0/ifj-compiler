# Makefile for running parser tests
# Located in the tests/ directory

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -I../inc -D_POSIX_C_SOURCE=200809L

# Directories
SRCDIR = ../src
TESTDIR = .

# Source files
SRC_FILES = \
    $(SRCDIR)/parser.c \
    $(SRCDIR)/lexer.c \
    $(SRCDIR)/ast.c \
    $(SRCDIR)/error.c \
    $(SRCDIR)/stack.c \
    $(SRCDIR)/ast_node_stack.c \
    $(SRCDIR)/token.c \
    $(SRCDIR)/keyword_htab.c  # Added keyword_htab.c

# Test files
TEST_FILES = \
    $(TESTDIR)/test_parser.c

# All source files
ALL_SRCS = $(SRC_FILES) $(TEST_FILES)

# Object files
OBJS = $(ALL_SRCS:.c=.o)

# Target executable
TARGET = test_parser

# Default rule
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
