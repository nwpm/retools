# === Info ===
PROG_NAME = rhead

# === Compiler ===
CC     = gcc
CFLAGS = -g -O0 -fsanitize=address -Wall -Wextra -pedantic -std=gnu99

DEBUG_FLAGS   = -g -O0 -fsanitize=address
RELEASE_FLAGS = -O2

# === Paths ===

BUILD_PATH  = ../../bin/rhead

RHEAD_SRC     = rhead.c
RHEAD_OBJ     = $(BUILD_PATH)/$(PROG_NAME).o
RHEAD_TARGET  = $(BUILD_PATH)/$(PROG_NAME)

# ===Targets===
.PHONY: rhead

rhead: $(RHEAD_TARGET) 

# ===Build rules===

$(RHEAD_TARGET) : $(RHEAD_OBJ)
	$(CC) $(CFLAGS) $< -o $@
	rm $(RHEAD_OBJ)

$(RHEAD_OBJ) : $(RHEAD_SRC) $(BUILD_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_PATH):
	mkdir -p $@


