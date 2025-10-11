// File: tests/balanced_test.c
#include "unity.h"
#include <stdio.h>
#include "stack.h"

// 구현은 다른 파일에 있다고 가정.
// 헤더가 있다면 #include "balanced.h" 로 바꾸세요.


// --- Optional: 짧은 헬퍼 매크로 ---
#define ASSERT_BALANCED(s)     TEST_ASSERT_EQUAL_INT_MESSAGE(1, balanced((char*)(s)), (s))
#define ASSERT_NOT_BALANCED(s) TEST_ASSERT_EQUAL_INT_MESSAGE(0, balanced((char*)(s)), (s))

// setUp/tearDown: 현재 테스트는 별도 리소스 사용 안 함
void setUp(void) {}
void tearDown(void) {}

// ------------------ 테스트 케이스 ------------------

// 기본 균형 케이스
void test_simple_pairs(void) {
    ASSERT_BALANCED("()");
    ASSERT_BALANCED("[]");
    ASSERT_BALANCED("{}");
}

// 중첩과 조합
void test_nested_and_concatenated(void) {
    ASSERT_BALANCED("([])");
    ASSERT_BALANCED("{[]()[]}");
    ASSERT_BALANCED("()[]{}");
    ASSERT_BALANCED("({[]})[]{}");
}

// 불균형: 조기 불일치/짝 안 맞음
void test_mismatch_and_early_close(void) {
    ASSERT_NOT_BALANCED("{[)]}");
    ASSERT_NOT_BALANCED("[({{)])");   // 문제에서 제시한 예
    ASSERT_NOT_BALANCED("([)]");
    ASSERT_NOT_BALANCED("())");
    ASSERT_NOT_BALANCED("]{");
}

// 불균형: 여는 괄호만 남음 / 닫는 괄호만 초과
void test_extra_open_or_close(void) {
    ASSERT_NOT_BALANCED("(((");
    ASSERT_NOT_BALANCED("{{)]");
    ASSERT_NOT_BALANCED("())]");
}

// 빈 문자열 처리(일반적으로 ‘균형’으로 간주)
void test_empty_string(void) {
    ASSERT_BALANCED("");
}

// 길고 복잡하지만 균형인 케이스
void test_long_balanced_sequence(void) {
    ASSERT_BALANCED("{{[()()][]}{()()[[]]}}");
}

// 경계: 같은 타입 반복
void test_repeated_same_type(void) {
    ASSERT_BALANCED("((((((((()))))))))");
    ASSERT_NOT_BALANCED("(((((((()))))))"); // 하나 부족
}

// 메인 러너
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_simple_pairs);
    RUN_TEST(test_nested_and_concatenated);
    RUN_TEST(test_mismatch_and_early_close);
    RUN_TEST(test_extra_open_or_close);
    RUN_TEST(test_empty_string);
    RUN_TEST(test_long_balanced_sequence);
    RUN_TEST(test_repeated_same_type);

    return UNITY_END();
}
