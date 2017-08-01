#include <hal/hal>
#include "atomic.h"
#include "unity.h"

using namespace avr;

#define CHECK_INTS_ON() TEST_ASSERT_TRUE(hal::libs::read_bit<SREG_I>(SREG))
#define CHECK_INTS_OFF() TEST_ASSERT_FALSE(hal::libs::read_bit<SREG_I>(SREG))

struct TestStruct {
    uint32_t value;
    void operator=(const TestStruct& rhs) {
        CHECK_INTS_OFF();
        value = rhs.value;
    }
};


Atomic<TestStruct> ts;

void test_Atomic_sei_cli() {
    cli();
    CHECK_INTS_OFF();

    sei();
    CHECK_INTS_ON();

    cli();
    CHECK_INTS_OFF();
}

void test_Atomic_DisablesDuringWrite() {
    TestStruct test;

    sei();
    ts = test;
}

void test_Atomic_ForceOn() {
    TestStruct test;

    cli();
    ts = test;
    CHECK_INTS_ON();
}

void test_Atomic_CheckResult() {
    TestStruct test;
    test.value = (uint32_t)0xDEADBEEF;

    ts = test;

    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, static_cast<TestStruct>(ts).value);
}


void test_Atomic() {
    UnityBegin("");

    RUN_TEST(test_Atomic_sei_cli);
    RUN_TEST(test_Atomic_DisablesDuringWrite);
    RUN_TEST(test_Atomic_ForceOn);
    RUN_TEST(test_Atomic_CheckResult);

    UnityEnd();
}
