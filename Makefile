CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99 -Iinclude
LDFLAGS =

SRC_DIR  = src
OBJ_DIR  = obj
BIN      = fleet_mgmt

SRCS     = $(wildcard $(SRC_DIR)/*.c)
OBJS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean run tests

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

data:
	mkdir -p data

run: all data
	./$(BIN)

# ---------- unit tests ----------
TEST_DIR     = tests
TEST_SRCS    = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS    = $(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/%, $(TEST_SRCS))

# Build helper: compile a single test binary linked against all modules
# except main.c
MODULE_SRCS  = $(filter-out $(SRC_DIR)/main.c, $(SRCS))
MODULE_OBJS  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(MODULE_SRCS))

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(MODULE_OBJS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^

tests: $(TEST_BINS)
	@pass=0; fail=0; \
	for t in $(TEST_BINS); do \
		echo "--- Running $$t ---"; \
		if ./$$t; then pass=$$((pass+1)); else fail=$$((fail+1)); fi; \
	done; \
	echo ""; \
	echo "Results: $$pass passed, $$fail failed."; \
	[ $$fail -eq 0 ]

clean:
	rm -rf $(OBJ_DIR) $(BIN) $(TEST_BINS) data
