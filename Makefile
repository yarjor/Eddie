.DEFAULT_GOAL := eddie

INCLUDE_DIR=./include
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I$(INCLUDE_DIR)
MATH_FLAGS = -lm
OUTPUT_DIR="./out"
OUTPUT_NAME = eddie
DEBUG_FLAGS = -D VSCODE -ggdb
C_FILES = eddie.c terminal.c buffer.c editor.c file.c search.c highlight.c

eddie: $(C_FILES)
	$(CC) $(C_FILES) -o $(OUTPUT_DIR)/$(OUTPUT_NAME) $(CFLAGS) $(MATH_FLAGS)

debug: $(C_FILES)
	$(CC) $(C_FILES) -o $(OUTPUT_DIR)/$(OUTPUT_NAME) $(CFLAGS) $(MATH_FLAGS) $(DEBUG_FLAGS)
	