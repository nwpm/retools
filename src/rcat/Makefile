# === Info ===
PROG_NAME = rcat

# === Compiler ===
CC     = gcc
CFLAGS = -g -O0 -fsanitize=address -Wall -Wextra -pedantic -std=gnu99

DEBUG_FLAGS   = -g -O0 -fsanitize=address
RELEASE_FLAGS = -O2

# === Paths ===

BUILD_PATH  = ../../bin/rcat

RCAT_SRC     = rcat.c
RCAT_OBJ     = $(BUILD_PATH)/$(PROG_NAME).o
RCAT_TARGET  = $(BUILD_PATH)/$(PROG_NAME)

# ===Targets===
.PHONY: rcat

rcat: $(RCAT_TARGET) 

# ===Build rules===

$(RCAT_TARGET) : $(RCAT_OBJ)
	$(CC) $(CFLAGS) $< -o $@
	rm $(RCAT_OBJ)

$(RCAT_OBJ) : $(RCAT_SRC) $(BUILD_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_PATH):
	mkdir -p $@

