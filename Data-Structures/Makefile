CC = gcc
CFLAGS = -I. -I./tests -Wall -g

# unity.c is always included
UNITY = tests/unity.c

# Separate test files
TEST_A1 = tests/linked_list_a1_test.c
TEST_A2 = tests/linked_list_a2_test.c
TEST_A3 = tests/linked_list_a3_test.c
TEST_A4 = tests/linked_list_a4_test.c
TEST_A5 = tests/linked_list_a5_test.c
TEST_A6 = tests/linked_list_a6_test.c
TEST_A7 = tests/linked_list_a7_test.c

TEST_C1 = tests/stack_queue_c1_test.c

# Implementation files
SRCS_LINKED_LIST = tests/linked_list.c
SRCS_QUEUE = tests/queue.c


# Separate targets
# Linked List
TARGET_A1 = test_runner_a1
TARGET_A2 = test_runner_a2
TARGET_A3 = test_runner_a3
TARGET_A4 = test_runner_a4
TARGET_A5 = test_runner_a5
TARGET_A6 = test_runner_a6
TARGET_A7 = test_runner_a7

# Queue
TARGET_C1 = test_runner_c1

# Test A1 only
test_a1:
	$(CC) $(CFLAGS) -o $(TARGET_A1) $(UNITY) $(TEST_A1) $(SRCS_LINKED_LIST)
	./$(TARGET_A1)

# Test A2 only
test_a2:
	$(CC) $(CFLAGS) -o $(TARGET_A2) $(UNITY) $(TEST_A2) $(SRCS_LINKED_LIST)
	./$(TARGET_A2)

test_a3:
	$(CC) $(CFLAGS) -o $(TARGET_A3) $(UNITY) $(TEST_A3) $(SRCS_LINKED_LIST)
	./$(TARGET_A3)

test_a4:
	$(CC) $(CFLAGS) -o $(TARGET_A4) $(UNITY) $(TEST_A4) $(SRCS_LINKED_LIST)
	./$(TARGET_A4)

test_a5:
	$(CC) $(CFLAGS) -o $(TARGET_A5) $(UNITY) $(TEST_A5) $(SRCS_LINKED_LIST)
	./$(TARGET_A5)

test_a6:
	$(CC) $(CFLAGS) -o $(TARGET_A6) $(UNITY) $(TEST_A6) $(SRCS_LINKED_LIST)
	./$(TARGET_A6)

test_a7:
	$(CC) $(CFLAGS) -o $(TARGET_A7) $(UNITY) $(TEST_A7) $(SRCS_LINKED_LIST)
	./$(TARGET_A7)

test_c1:
	$(CC) $(CFLAGS) -o $(TARGET_C1) $(UNITY) $(TEST_C1) $(SRCS_LINKED_LIST) $(SRCS_QUEUE)
	./$(TARGET_C1)

# Run all tests sequentially
test: test_a1 test_a2 test_a3 test_a4 test_a5 test_a6 test_a7 test_c1

clean:
	rm -f $(TARGET_A1) $(TARGET_A2) $(TARGET_A3) $(TARGET_A4) $(TARGET_A5) $(TARGET_A6) $(TARGET_A7) $(TARGET_C1)

.PHONY: test test_a1 test_a2 test_a3 test_a4 test_a5 test_a6 test_a7 test_c1 clean
