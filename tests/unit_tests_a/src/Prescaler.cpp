#include <unity.h>
#include <hal/hal>

#include "Prescaler.h"

using namespace avr;

void test_Prescaler_initialise() {
    {
        avr::Prescaler<2> prescaler;
        TEST_ASSERT_FALSE(prescaler.expired());
    }
    {
        avr::Prescaler<100> prescaler;
        for (int i = 0; i < 99; ++i) {
            TEST_ASSERT_FALSE(prescaler.expired());
        }
    }
}

void test_Prescaler_no_prescale() {
    avr::Prescaler<1> prescaler;

    for (int i = 0; i < 1000; ++i) {
        TEST_ASSERT_TRUE(prescaler.expired());
    }
}

template<uint64_t n>
void test_Prescaler_expire_retry_impl() {
    avr::Prescaler<n> prescaler;

    for (int r = 0; r < 10; ++r) {
        for (uint64_t i = 0; i < n - 1; ++i) {
            TEST_ASSERT_FALSE(prescaler.expired());
        }
        TEST_ASSERT_TRUE(prescaler.expired());
    }
}

void test_Prescaler_expire_retry() {
    test_Prescaler_expire_retry_impl<1>();
    test_Prescaler_expire_retry_impl<100>();

    test_Prescaler_expire_retry_impl<1000>();
}

void test_Prescaler() {
    UnityBegin("");

    RUN_TEST(test_Prescaler_initialise);
    RUN_TEST(test_Prescaler_no_prescale);
    RUN_TEST(test_Prescaler_expire_retry);

    UnityEnd();
}
