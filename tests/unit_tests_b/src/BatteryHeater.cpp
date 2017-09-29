#include <hal/hal>
#include "unity.h"

#include "BatteryHeater.h"

template<int tag>
struct PinMock {
    static bool value;

    static bool read() {
        return value;
    }

    static void set() {
        value = true;
    }
    static void reset() {
        value = false;
    }
    static void check(bool desired) {
        TEST_ASSERT_EQUAL(desired, value);
    }
};
template<int tag>
bool PinMock<tag>::value;

using PinHigh = PinMock<10>;
using PinLow  = PinMock<11>;

using namespace eps::battery_heater;
using ActiveHigh = BatteryHeater<PinHigh, LogicLevel::ActiveHigh>;
using ActiveLow  = BatteryHeater<PinLow, LogicLevel::ActiveLow>;

void test_BatteryHeater_high_switching() {
    ActiveHigh::enable();
    PinHigh::check(true);
    TEST_ASSERT_TRUE(ActiveHigh::heating());

    ActiveHigh::disable();
    PinHigh::check(false);
    TEST_ASSERT_FALSE(ActiveHigh::heating());
}

void test_BatteryHeater_low_switching() {
    ActiveLow::enable();
    PinLow::check(false);
    TEST_ASSERT_TRUE(ActiveLow::heating());

    ActiveLow::disable();
    PinLow::check(true);
    TEST_ASSERT_FALSE(ActiveLow::heating());
}


void test_BatteryHeater() {
    UnityBegin("");

    RUN_TEST(test_BatteryHeater_high_switching);
    RUN_TEST(test_BatteryHeater_low_switching);

    UnityEnd();
}
