.PHONY: test clean

CFLAGS = -I ../src -Wall -g -DRBTREE_TESTING -DSENTINEL
CC = cc

test:test-unity-rbtree
	./test-unity-rbtree
	valgrind ./test-unity-rbtree
	test-rbtree

test-rbtree: test-rbtree.c 	../src/rbtree.c

test-unity-rbtree: test-unity-rbtree.c unity.c ../src/rbtree.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f test-unity-rbtree *.o

# OLD Test makefile
# .PHONY: test

# CFLAGS=-I ../src -Wall -g #-DSENTINEL

# test: test-rbtree
# 	./test-rbtree
# 	valgrind ./test-rbtree

# test-rbtree: test-rbtree.o ../src/rbtree.o

# ../src/rbtree.o:
# 	$(MAKE) -C ../src rbtree.o

# clean:
# 	rm -f test-rbtree *.o
