#include "SoftwareTimer.h"
#include <hal/hal>
#include "unity.h"

using namespace avr;

struct Callbacks {
    static uint16_t last_countdown;
    static bool last_expired;

    static void countdown(uint16_t count) {
        last_countdown = count;
    }
    static void expired() {
        last_expired = true;
    }
};
uint16_t Callbacks::last_countdown;
bool Callbacks::last_expired;


using timer_10    = SoftwareTimer<Callbacks, 10>;
using timer_50000 = SoftwareTimer<Callbacks, 50000>;


void test_SoftwareTimer_default_expired() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_10::tick();

    TEST_ASSERT_EQUAL_INT(0, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(true, Callbacks::last_expired);
}

void test_SoftwareTimer_reset_does_not_invoke_callbacks() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_10::reset();

    TEST_ASSERT_EQUAL_INT(0, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
}

void test_SoftwareTimer_counts_from_TOP() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_10::reset();
    timer_10::tick();

    TEST_ASSERT_EQUAL_INT(9, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
}

void test_SoftwareTimer_full_cycle() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_10::reset();
    for (int i = 9; i > 0; --i) {
        timer_10::tick();
        TEST_ASSERT_EQUAL_INT(i, Callbacks::last_countdown);
        TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
    }

    timer_10::tick();
    TEST_ASSERT_EQUAL_INT(0, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(true, Callbacks::last_expired);
}

void test_SoftwareTimer_reset_before_timeout() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_10::reset();
    for (int i = 9; i > 1; --i) {
        timer_10::tick();
        TEST_ASSERT_EQUAL_INT(i, Callbacks::last_countdown);
        TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
    }

    timer_10::reset();
    for (int i = 9; i > 0; --i) {
        timer_10::tick();
        TEST_ASSERT_EQUAL_INT(i, Callbacks::last_countdown);
        TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
    }
    timer_10::tick();
    TEST_ASSERT_EQUAL_INT(0, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(true, Callbacks::last_expired);
}

void test_SoftwareTimer_long_timeout() {
    Callbacks::last_countdown = 0;
    Callbacks::last_expired   = false;

    timer_50000::reset();
    for (uint16_t i = 49999; i > 0; --i) {
        timer_50000::tick();
        TEST_ASSERT_EQUAL_INT(i, Callbacks::last_countdown);
        TEST_ASSERT_EQUAL(false, Callbacks::last_expired);
    }

    timer_50000::tick();
    TEST_ASSERT_EQUAL_INT(0, Callbacks::last_countdown);
    TEST_ASSERT_EQUAL(true, Callbacks::last_expired);
}

void test_SoftwareTimer() {
    UnityBegin("");

    RUN_TEST(test_SoftwareTimer_default_expired);
    RUN_TEST(test_SoftwareTimer_reset_does_not_invoke_callbacks);
    RUN_TEST(test_SoftwareTimer_counts_from_TOP);
    RUN_TEST(test_SoftwareTimer_full_cycle);
    RUN_TEST(test_SoftwareTimer_reset_before_timeout);
    RUN_TEST(test_SoftwareTimer_long_timeout);

    UnityEnd();
}
