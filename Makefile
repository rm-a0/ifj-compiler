# Compiler and Flags
CC = gcc
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g -O3 -pedantic 

# Directories
SRC_DIR = src
INC_DIR = inc
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

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

zip:
	zip -r ifj.zip $(SRC_DIR) $(INC_DIR)

.PHONY: all run clean zip
