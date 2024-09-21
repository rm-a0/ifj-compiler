CC = gcc
CFLAGS = -I$(INC_DIR) -Wall -Wextra -g -O3 -pedantic -fsanitize=address

SRC_DIR = src
INC_DIR = inc

TARGET = main

SRC_FILES = $(SRC_DIR)/example.c
INC_FILES = $(INC_DIR)/example.h

# Rules
all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

zip:
	zip -r ifj.zip $(SRC_DIR) $(INC_DIR)

.PHONY: all run clean zip
