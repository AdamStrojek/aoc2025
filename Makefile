CC = clang
CFLAGS = -std=c23 -Wall -Wextra -pedantic -ggdb
BIN_DIR = bin

# Find all main.c files in day* directories
SOURCES = $(wildcard day*/main.c)
# Create executable names from the directory names, prefixed with the bin directory
EXECUTABLES = $(patsubst %/main.c,$(BIN_DIR)/%,$(SOURCES))

.PHONY: all clean

all: $(EXECUTABLES)

# Create the bin directory before building
$(EXECUTABLES): | $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to build an executable from a day's main.c
# $< is the first prerequisite (the .c file)
# $@ is the target (the executable name)
$(BIN_DIR)/%: %/main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BIN_DIR)
