#include "MainTimer.h"
#include <unity.h>

using namespace eps;
using namespace std::chrono_literals;

void test_MainTimer_interrupt() {
    MainTimer::init();

    sei();

    hal::sleep_for(100ms);
    TEST_ASSERT_TRUE(MainTimer::expired());
}

template<uint32_t timeout_us, bool should_expire>
void test_timeout() {
    MainTimer::init();

    _delay_us(timeout_us);
    TEST_ASSERT_EQUAL(should_expire, MainTimer::expired());
}

void test_MainTimer_duration() {
    MainTimer::init();

    test_timeout<1, false>();
    test_timeout<32000, false>();
    test_timeout<32200, false>();

    test_timeout<32300, true>();
    test_timeout<35000, true>();
    test_timeout<100000, true>();
}

void test_MainTimer_wakeup() {
    MainTimer::init();

    sleep_mode();

    TEST_PASS();
}

void test_MainTimer() {
    UnityBegin("");

    RUN_TEST(test_MainTimer_interrupt);
    RUN_TEST(test_MainTimer_duration);
    RUN_TEST(test_MainTimer_wakeup);

    UnityEnd();
}
