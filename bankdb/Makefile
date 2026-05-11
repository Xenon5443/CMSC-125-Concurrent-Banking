# CMSC 125 Lab 3 - Concurrent Banking System
# Makefile

# ----------------------------------------------------------
# Compiler
# ----------------------------------------------------------
CC = gcc

# ----------------------------------------------------------
# Directories
# ----------------------------------------------------------
SRC_DIR   = src
INC_DIR   = include
OBJ_DIR   = obj
DOBJ_DIR  = obj_debug
BIN_DIR   = bin
TEST_DIR  = tests

# ----------------------------------------------------------
# Executable Names
# ----------------------------------------------------------
TARGET       = $(BIN_DIR)/bankdb
DEBUG_TARGET = $(BIN_DIR)/bankdb_debug

# ----------------------------------------------------------
# Source / Object Files
# ----------------------------------------------------------
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Production objects  -> obj/
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Debug objects       -> obj_debug/   (kept separate to avoid contamination)
DOBJS = $(patsubst $(SRC_DIR)/%.c,$(DOBJ_DIR)/%.o,$(SRCS))

# ----------------------------------------------------------
# Compiler Flags
# ----------------------------------------------------------
COMMON_FLAGS = -I$(INC_DIR) -pthread -Wall -Wextra

CFLAGS       = $(COMMON_FLAGS) -O2
DEBUG_FLAGS  = $(COMMON_FLAGS) -g -fsanitize=thread -fno-omit-frame-pointer

# ----------------------------------------------------------
# Default Target: production build only, no TSan
# ----------------------------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------
# Debug / TSan build -> separate binary AND separate obj dir
# ----------------------------------------------------------
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DOBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(DEBUG_FLAGS) $^ -o $@

$(DOBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(DOBJ_DIR)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

# ----------------------------------------------------------
# Run Program
# ----------------------------------------------------------
run: all
	./$(TARGET)

# ==========================================================
# INDIVIDUAL TEST CASES  (all use production binary)
# ==========================================================

test1: all
	@echo "========================================="
	@echo "Running Test 1: Simple Transactions"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_simple.txt \
		--deadlock=prevention

test2: all
	@echo "========================================="
	@echo "Running Test 2: Concurrent Readers"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_readers.txt \
		--deadlock=prevention

test3: all
	@echo "========================================="
	@echo "Running Test 3: Deadlock Scenario"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_deadlock.txt \
		--deadlock=prevention

test4: all
	@echo "========================================="
	@echo "Running Test 4: Insufficient Funds"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_abort.txt \
		--deadlock=prevention

test5: all
	@echo "========================================="
	@echo "Running Test 5: Buffer Pool Saturation"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_buffer.txt \
		--deadlock=prevention

test6: all
	@echo "========================================="
	@echo "Running Test 6"
	@echo "========================================="
	./$(TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_standard.txt \
		--deadlock=prevention

test: test1 test2 test3 test4 test5 test6

# ----------------------------------------------------------
# ThreadSanitizer test  (uses debug binary explicitly)
# ----------------------------------------------------------
tsan: debug
	@echo "========================================="
	@echo "Running TSan: Concurrent Readers"
	@echo "========================================="
	setarch -R $(DEBUG_TARGET) \
		--accounts=$(TEST_DIR)/accounts.txt \
		--trace=$(TEST_DIR)/trace_readers.txt \
		--deadlock=prevention

# ----------------------------------------------------------
# Clean
# ----------------------------------------------------------
clean:
	rm -rf $(OBJ_DIR) $(DOBJ_DIR) $(BIN_DIR)

rebuild: clean all

.PHONY: all debug run \
	test test1 test2 test3 test4 test5 test6 \
	tsan clean rebuild