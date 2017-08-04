#include "MainTimer.h"
#include <unity.h>

using namespace std::chrono_literals;

void test_MainTimer_interrupt() {
    eps_a::MainTimer::init();

    sei();

    hal::sleep_for(100ms);
    TEST_ASSERT_TRUE(eps_a::MainTimer::expired());
}

template<uint32_t timeout_ms, bool should_expire>
void test_timeout() {
    hal::sleep_for(100ms);

    TEST_ASSERT_TRUE(eps_a::MainTimer::expired());
    _delay_ms(timeout_ms);
    TEST_ASSERT_EQUAL(should_expire, eps_a::MainTimer::expired());
}

void test_MainTimer_duration() {
    eps_a::MainTimer::init();

    test_timeout<1, false>();
    test_timeout<30, false>();
    test_timeout<33, false>();

    test_timeout<34, true>();
    test_timeout<50, true>();
    test_timeout<1000, true>();
}

void test_MainTimer() {
    UnityBegin("");

    RUN_TEST(test_MainTimer_interrupt);
    RUN_TEST(test_MainTimer_duration);

    UnityEnd();
}
