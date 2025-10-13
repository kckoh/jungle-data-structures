# -----------------------------------------------------------------------------
# Toolchain configuration
# -----------------------------------------------------------------------------
CC      := gcc
CFLAGS  := -Wall -g -I. -I./tests

UNITY_SRC := tests/unity.c

LIB_LINKED_LIST        := tests/linked_list.c
LIB_QUEUE              := tests/queue.c
LIB_STACK              := tests/stack.c
LIB_BINARY_TREE        := tests/binary_tree.c
LIB_BINARY_SEARCH_TREE := tests/binary_search_tree.c

# -----------------------------------------------------------------------------
# Test discovery
# -----------------------------------------------------------------------------
LL_TEST_SRCS  := $(wildcard tests/linked_list_a*_test.c)
SQ_TEST_SRCS  := $(wildcard tests/stack_queue_c*_test.c)
BT_TEST_SRCS  := $(wildcard tests/binary_tree_e*_test.c)
BST_TEST_SRCS := $(wildcard tests/binary_search_tree_f*_test.c)

LL_RUNNERS  := $(patsubst tests/%_test.c,test_runner_%_test,$(LL_TEST_SRCS))
SQ_RUNNERS  := $(patsubst tests/%_test.c,test_runner_%_test,$(SQ_TEST_SRCS))
BT_RUNNERS  := $(patsubst tests/%_test.c,test_runner_%_test,$(BT_TEST_SRCS))
BST_RUNNERS := $(patsubst tests/%_test.c,test_runner_%_test,$(BST_TEST_SRCS))

ALL_RUNNERS := $(LL_RUNNERS) $(SQ_RUNNERS) $(BT_RUNNERS) $(BST_RUNNERS)

# -----------------------------------------------------------------------------
# Phony targets
# -----------------------------------------------------------------------------
.PHONY: test clean run_%_test

test: $(ALL_RUNNERS)
	@echo "--- Running all tests sequentially ---"
	@for runner in $(ALL_RUNNERS); do \
		echo "-> Running $$runner..."; \
		./$$runner; \
	done
	@echo "--- All tests finished! ---"

clean:
	rm -f $(ALL_RUNNERS)

run_%_test: test_runner_%_test
	@echo "-> Running $<..."
	@./$<

# -----------------------------------------------------------------------------
# Build rules
# -----------------------------------------------------------------------------
test_runner_linked_list_%_test: tests/linked_list_%_test.c $(UNITY_SRC) $(LIB_LINKED_LIST)
	$(CC) $(CFLAGS) -o $@ $^

test_runner_stack_queue_%_test: tests/stack_queue_%_test.c $(UNITY_SRC) \
                                $(LIB_LINKED_LIST) $(LIB_QUEUE) $(LIB_STACK)
	$(CC) $(CFLAGS) -o $@ $^

test_runner_binary_tree_%_test: tests/binary_tree_%_test.c $(UNITY_SRC) $(LIB_BINARY_TREE)
	$(CC) $(CFLAGS) -o $@ $^

test_runner_binary_search_tree_%_test: tests/binary_search_tree_%_test.c $(UNITY_SRC) \
                                       $(LIB_BINARY_SEARCH_TREE)
	$(CC) $(CFLAGS) -o $@ $^

# Fallback rule for any remaining tests that only need the linked list helpers
test_runner_%_test: tests/%_test.c $(UNITY_SRC) $(LIB_LINKED_LIST)
	$(CC) $(CFLAGS) -o $@ $^
