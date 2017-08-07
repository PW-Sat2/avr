#include <unity.h>

#include "ObcWatchdog.h"

static constexpr auto timeout = 10 * 60;

static bool full_power_cycle_requested = false;

static void callback() {
    full_power_cycle_requested = true;
}

using wdt = eps_a::ObcWatchdog<callback>;

void test_ObcWatchdog_init() {
    TEST_ASSERT_FALSE(full_power_cycle_requested);
    wdt::tick();
    TEST_ASSERT_TRUE(full_power_cycle_requested);
}

void test_ObcWatchdog_kick_and_expire() {
    for (int times = 0; times < 5; ++times) {
        full_power_cycle_requested = false;

        wdt::kick();

        for (int i = 0; i < timeout; ++i) {
            TEST_ASSERT_FALSE(full_power_cycle_requested);
            wdt::tick();
        }
        TEST_ASSERT_TRUE(full_power_cycle_requested);
    }
}

void test_ObcWatchdog_kick_before_expiration() {
    full_power_cycle_requested = false;

    wdt::kick();

    for (int i = 0; i < timeout - 1; ++i) {
        TEST_ASSERT_FALSE(full_power_cycle_requested);
        wdt::tick();
    }
    TEST_ASSERT_FALSE(full_power_cycle_requested);

    wdt::kick();

    for (int i = 0; i < timeout - 1; ++i) {
        TEST_ASSERT_FALSE(full_power_cycle_requested);
        wdt::tick();
    }
    TEST_ASSERT_FALSE(full_power_cycle_requested);
    wdt::tick();

    TEST_ASSERT_TRUE(full_power_cycle_requested);
}

void test_ObcWatchdog() {
    UnityBegin("");

    RUN_TEST(test_ObcWatchdog_init);
    RUN_TEST(test_ObcWatchdog_kick_and_expire);
    RUN_TEST(test_ObcWatchdog_kick_before_expiration);

    UnityEnd();
}
