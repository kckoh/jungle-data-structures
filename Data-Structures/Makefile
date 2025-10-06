CC = gcc
CFLAGS = -I. -I./tests -Wall -g

# unity.c는 항상 포함
UNITY = tests/unity.c

# tests/ 폴더에서 "test"가 이름에 들어간 .c 파일 자동 검색
TESTS = $(wildcard tests/*test*.c)

# 실제 구현 파일들 (필요한 .c를 여기에 추가)
SRCS = tests/linked_list.c 

TARGET = test_runner

test:
	$(CC) $(CFLAGS) -o $(TARGET) $(UNITY) $(TESTS) $(SRCS)
	./$(TARGET)

clean:
	rm -f $(TARGET)