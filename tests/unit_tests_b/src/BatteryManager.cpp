#include <hal/hal>
#include "unity.h"

#include "BatteryManager.h"

using namespace avr;


struct PinMock {
    static bool value;

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
bool PinMock::value;

using PinCharge = PinMock;

BatteryManager<PinMock> battery_manager;

void test_BatteryManager_default_state() {
    battery_manager.tick(0, 1000);

    PinCharge::check(false);
}

constexpr static float charge_high = 8.0;
constexpr static float charge_low  = 7.8;
constexpr static float charge_mid  = (charge_high + charge_low) / 2.0;

void test_BatteryManager_charging_voltage() {
    for (float i = 0; i < charge_high; i += 0.1) {
        battery_manager.tick(i, 0);
        PinCharge::check(true);
    }

    battery_manager.tick(charge_high + 0.01, 0);
    PinCharge::check(false);

    for (float i = charge_high; i >= charge_low; i -= 0.1) {
        battery_manager.tick(i, 0);
        PinCharge::check(false);
    }

    battery_manager.tick(charge_low - 0.1, 0);
    PinCharge::check(true);

    battery_manager.tick(charge_mid, 0);
    PinCharge::check(true);

    battery_manager.tick(charge_high + 0.1, 0);
    PinCharge::check(false);

    battery_manager.tick(charge_mid, 0);
    PinCharge::check(false);
}

constexpr static float charge_temp_high = 43;
constexpr static float charge_temp_low  = 40;

void test_BatteryManager_charging_temperature() {
    for (float t = 0; t < charge_temp_high; t += 0.1) {
        battery_manager.tick(0, t);
        PinCharge::check(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 0.1) {
        battery_manager.tick(0, t);
        PinCharge::check(false);
    }
    for (float t = charge_temp_low - 0.1; t <= charge_temp_high; t += 0.1) {
        battery_manager.tick(0, t);
        PinCharge::check(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 0.1) {
        battery_manager.tick(0, t);
        PinCharge::check(false);
    }

    battery_manager.tick(0, charge_temp_low - 0.1);
    PinCharge::check(true);
}


void test_BatteryManager() {
    UnityBegin("");

    RUN_TEST(test_BatteryManager_default_state);
    RUN_TEST(test_BatteryManager_charging_voltage);
    RUN_TEST(test_BatteryManager_charging_temperature);

    UnityEnd();
}