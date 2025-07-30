# === Info ===
PROG_NAME = rtail

# === Compiler ===
CC     = gcc
CFLAGS = -g -O0 -fsanitize=address -Wall -Wextra -pedantic -std=gnu99

DEBUG_FLAGS   = -g -O0 -fsanitize=address
RELEASE_FLAGS = -O2

# === Paths ===

BUILD_PATH  = ../../bin/rtail

RTAIL_SRC     = rtail.c
RTAIL_OBJ     = $(BUILD_PATH)/$(PROG_NAME).o
RTAIL_TARGET  = $(BUILD_PATH)/$(PROG_NAME)

# ===Targets===
.PHONY: rtail

rtail: $(RTAIL_TARGET) 

# ===Build rules===

$(RTAIL_TARGET) : $(RTAIL_OBJ)
	$(CC) $(CFLAGS) $< -o $@
	rm $(RTAIL_OBJ)

$(RTAIL_OBJ) : $(RTAIL_SRC) $(BUILD_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_PATH):
	mkdir -p $@



