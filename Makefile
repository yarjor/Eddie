INCLUDE_DIR=./include
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I$(INCLUDE_DIR) -ggdb
OUTPUT_DIR="./out"
OUTPUT_NAME = kilo
C_FILES = kilo.c terminal.c buffer.c editor.c file.c search.c highlight.c

kilo: $(C_FILES)
	$(CC) $(C_FILES) -o $(OUTPUT_DIR)/$(OUTPUT_NAME) $(CFLAGS)
