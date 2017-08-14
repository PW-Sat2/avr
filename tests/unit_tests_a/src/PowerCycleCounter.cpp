#include "unity.h"

#include "PowerCycleCounter.h"

using namespace avr;
using namespace avr::power_cycle_counters;

static bool deployed = false;
static void deploy() {
    deployed = true;
}

uint16_t all_counter = 0;

void bootup() {
    all_counter++;
    bootup(deploy);
}

void test_PowerCycleCounter_init() {
    TEST_ASSERT_EQUAL(0, get().all);
    TEST_ASSERT_EQUAL(0, get().safety);
}

void test_PowerCycleCounter_increments() {
    reset();

    for (int i = 0; i <= 193; ++i) {
        TEST_ASSERT_EQUAL(all_counter, get().all);
        TEST_ASSERT_EQUAL(i, get().safety);

        bootup();
    }

    for (int i = 194; i <= 300; ++i) {
        TEST_ASSERT_EQUAL(all_counter, get().all);
        TEST_ASSERT_EQUAL(193, get().safety);

        bootup();
    }
}

void test_PowerCycleCounter_deployment() {
    reset();
    deployed = false;

    for (int i = 0; i <= 192; ++i) {
        TEST_ASSERT_EQUAL(all_counter, get().all);
        TEST_ASSERT_EQUAL(i, get().safety);

        bootup();

        TEST_ASSERT_FALSE(deployed);
    }

    for (int i = 193; i <= 300; ++i) {
        TEST_ASSERT_EQUAL(all_counter, get().all);
        TEST_ASSERT_EQUAL(193, get().safety);

        deployed = false;

        bootup();

        TEST_ASSERT_TRUE(deployed);
    }
}

void test_PowerCycleCounter() {
    UnityBegin("");

    RUN_TEST(test_PowerCycleCounter_init);
    RUN_TEST(test_PowerCycleCounter_increments);
    RUN_TEST(test_PowerCycleCounter_deployment);

    UnityEnd();
}
