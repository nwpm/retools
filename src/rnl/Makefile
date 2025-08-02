# === Info ===
PROG_NAME = rnl

# === Compiler ===
CC     = gcc
CFLAGS = -g -O0 -fsanitize=address -Wall -Wextra -pedantic -std=gnu99

DEBUG_FLAGS   = -g -O0 -fsanitize=address
RELEASE_FLAGS = -O2

# === Paths ===

BUILD_PATH  = ../../bin/rnl

RNL_SRC     = rnl.c
RNL_OBJ     = $(BUILD_PATH)/$(PROG_NAME).o
RNL_TARGET  = $(BUILD_PATH)/$(PROG_NAME)

# ===Targets===
.PHONY: rnl

rnl: $(RNL_TARGET) 

# ===Build rules===

$(RNL_TARGET) : $(RNL_OBJ)
	$(CC) $(CFLAGS) $< -o $@
	rm $(RNL_OBJ)

$(RNL_OBJ) : $(RNL_SRC) $(BUILD_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_PATH):
	mkdir -p $@


