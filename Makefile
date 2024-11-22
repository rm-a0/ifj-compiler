# Compiler and Flags
CC = gcc
CFLAGS = -I$(INC_DIR) -g -O3 #-pedantic -fsanitize=address

# Directories
SRC_DIR = .
INC_DIR = .

# Target
TARGET = main

# Source and Include Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
INC_FILES = $(wildcard $(INC_DIR)/*.h)

# Object Files
OBJ_FILES = $(patsubst %.c,%.o,$(notdir $(SRC_FILES)))

# Rules
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(TARGET)

%.o: $(SRC_DIR)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

zip:
	zip -r xrepcim00.zip Makefile *.c *.h rozdeleni dokumentace.pdf rozsireni

cleanzip:
	rm -f xrepcim00.zip

.PHONY: all run clean zip