#include <array>

#include "unity.h"

#include "redundancy.h"

using std::uint8_t;
using std::uint32_t;

using namespace avr::redundancy;

void test_RedundancyTest3_ShouldFixSingleByte() {
    TEST_ASSERT_EQUAL(0b0000000, Correct(0b0000000, 0b0000000, 0b0000000));
    TEST_ASSERT_EQUAL(0b0000000, Correct(0b0000001, 0b0000000, 0b0000000));
    TEST_ASSERT_EQUAL(0b0000000, Correct(0b0000000, 0b0000001, 0b0000000));
    TEST_ASSERT_EQUAL(0b0000000, Correct(0b0000000, 0b0000000, 0b0000001));
    TEST_ASSERT_EQUAL(0b0000001, Correct(0b0000001, 0b0000001, 0b0000000));
    TEST_ASSERT_EQUAL(0b0000001, Correct(0b0000001, 0b0000000, 0b0000001));
    TEST_ASSERT_EQUAL(0b0000001, Correct(0b0000000, 0b0000001, 0b0000001));
    TEST_ASSERT_EQUAL(0b0000001, Correct(0b0000001, 0b0000001, 0b0000001));

    TEST_ASSERT_EQUAL(0b0010000, Correct(0b0010000, 0b0010000, 0b0010000));
    TEST_ASSERT_EQUAL(0b0010000, Correct(0b0000001, 0b0010000, 0b0010000));
    TEST_ASSERT_EQUAL(0b0010000, Correct(0b0010000, 0b0010001, 0b0000000));
    TEST_ASSERT_EQUAL(0b1010001, Correct(0b1010101, 0b1110000, 0b0010001));
}

void test_RedundancyTest5_ShouldFixSingleByte() {
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b0, 0b0, 0b0));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b0, 0b0, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b0, 0b1, 0b0));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b0, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b1, 0b0, 0b0));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b1, 0b0, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b0, 0b1, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b0, 0b0, 0b1, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b1, 0b0, 0b0, 0b0));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b1, 0b0, 0b0, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b1, 0b0, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b0, 0b1, 0b0, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b0, 0b1, 0b1, 0b0, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b0, 0b1, 0b1, 0b0, 0b1));
    TEST_ASSERT_EQUAL(1, Correct(0b0, 0b1, 0b1, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b0, 0b1, 0b1, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b1, 0b0, 0b0, 0b0, 0b0));
    TEST_ASSERT_EQUAL(0, Correct(0b1, 0b0, 0b0, 0b0, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b1, 0b0, 0b0, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b0, 0b0, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b1, 0b0, 0b1, 0b0, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b0, 0b1, 0b0, 0b1));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b0, 0b1, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b0, 0b1, 0b1, 0b1));
    TEST_ASSERT_EQUAL(0, Correct(0b1, 0b1, 0b0, 0b0, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b0, 0b0, 0b1));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b0, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b0, 0b1, 0b1));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b1, 0b0, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b1, 0b0, 0b1));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b1, 0b1, 0b0));
    TEST_ASSERT_EQUAL(1, Correct(0b1, 0b1, 0b1, 0b1, 0b1));
}

void test_RedundancyTest() {
    UnityBegin("");

    RUN_TEST(test_RedundancyTest3_ShouldFixSingleByte);
    RUN_TEST(test_RedundancyTest5_ShouldFixSingleByte);

    UnityEnd();
}
