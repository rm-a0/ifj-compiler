# Compiler and Flags
CC = gcc
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g -O3 #-pedantic -fsanitize=address

# Directories
SRC_DIR = .
INC_DIR = .
BUILD_DIR = build

# Target
TARGET = main

# Source and Include Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
INC_FILES = $(wildcard $(INC_DIR)/*.h)

# Object Files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Rules
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

zip:
	zip -r xrepcim00.zip Makefile *.c *.h rozdeleni dokumentace.pdf rozsireni

cleanzip:
	rm -r xrepcim00.zip

.PHONY: all run clean zip
