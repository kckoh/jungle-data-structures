CC = gcc
CFLAGS = -I. -I./tests -Wall -g

# unity.c is always included
UNITY = tests/unity.c

# Separate test files
TEST_A1 = tests/linked_list_a1_test.c
TEST_A2 = tests/linked_list_a2_test.c
TEST_A3 = tests/linked_list_a3_test.c
TEST_A4 = tests/linked_list_a4_test.c

# Implementation files
SRCS = tests/linked_list.c

# Separate targets
TARGET_A1 = test_runner_a1
TARGET_A2 = test_runner_a2
TARGET_A3 = test_runner_a3
TARGET_A4 = test_runner_a4

# Test A1 only
test_a1:
	$(CC) $(CFLAGS) -o $(TARGET_A1) $(UNITY) $(TEST_A1) $(SRCS)
	./$(TARGET_A1)

# Test A2 only
test_a2:
	$(CC) $(CFLAGS) -o $(TARGET_A2) $(UNITY) $(TEST_A2) $(SRCS)
	./$(TARGET_A2)

test_a3:
	$(CC) $(CFLAGS) -o $(TARGET_A3) $(UNITY) $(TEST_A3) $(SRCS)
	./$(TARGET_A3)

test_a4:
	$(CC) $(CFLAGS) -o $(TARGET_A4) $(UNITY) $(TEST_A4) $(SRCS)
	./$(TARGET_A4)

# Run all tests sequentially
test: test_a1 test_a2 test_a3 test_a4

clean:
	rm -f $(TARGET_A1) $(TARGET_A2) $(TARGET_A3) $(TARGET_A4)

.PHONY: test test_a1 test_a2  test_a3 test_a4 clean
