cmake_minimum_required(VERSION 3.10)
project(RBTreeProject C)

# Include src directory
include_directories(src)

# Collect source files
add_executable(test_rbtree
    #src/driver.c
    test/unity.c
    #test/test-rbtree.c
    test/test-unity-rbtree.c
    src/rbtree.c
)
