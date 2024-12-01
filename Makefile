# Compiler and Flags
CC = gcc
CFLAGS = -I$(INC_DIR) -g -O3 -pedantic #-fsanitize=address

# Directories
SRC_DIR = src/
INC_DIR = inc/

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
	sed 's/^SRC_DIR.*/SRC_DIR := ./' Makefile | sed 's/^INC_DIR.*/INC_DIR := ./' > Makefile.tmp && \
	mv Makefile.tmp Makefile && \
	zip -j xrepcim00.zip Makefile src/*.c inc/*.h doc/rozdeleni doc/dokumentace.pdf doc/rozsireni && \
	git checkout -- Makefile

cleanzip:
	rm -f xrepcim00.zip

test:
	./is_it_ok.sh xrepcim00.zip testdir

cleantest:
	rm -rf testdir

.PHONY: all run clean zip cleanzip