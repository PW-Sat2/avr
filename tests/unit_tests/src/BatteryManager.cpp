#include <hal/hal>
#include "unity.h"

#include "BatteryManager.h"

using namespace avr;

template<int tag>
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
template<int tag>
bool PinMock<tag>::value;

using PinCharge    = PinMock<0>;
using PinDischarge = PinMock<1>;
using PinHeater    = PinMock<2>;

BatteryManager<PinCharge, PinDischarge, PinHeater> battery_manager;

void test_BatteryManager_default_state() {
    battery_manager.tick(0, 1000);

    PinCharge::check(false);
}

constexpr static float charge_high = 8.0;
constexpr static float charge_low  = 7.8;
constexpr static float charge_mid  = (charge_high + charge_low) / 2.0;

void test_BatteryManager_charging_voltage() {
    for (float i = 0; i < charge_high; i += 1) {
        battery_manager.tick(i, 0);
        PinCharge::check(true);
    }

    battery_manager.tick(charge_high + 0.01, 0);
    PinCharge::check(false);

    for (float i = charge_high; i >= charge_low; i -= 1) {
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
    for (float t = 0; t < charge_temp_high; t += 1) {
        battery_manager.tick(0, t);
        PinCharge::check(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        PinCharge::check(false);
    }
    for (float t = charge_temp_low - 0.1; t <= charge_temp_high; t += 1) {
        battery_manager.tick(0, t);
        PinCharge::check(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        PinCharge::check(false);
    }

    battery_manager.tick(0, charge_temp_low - 0.1);
    PinCharge::check(true);
}

constexpr static float discharge_low  = 6.3;
constexpr static float discharge_high = 6.8;
constexpr static float discharge_mid  = (discharge_high + discharge_low) / 2.0;

void test_BatteryManager_discharging_voltage() {
    for (float i = 0; i < discharge_high; i += 1) {
        battery_manager.tick(i, 0);
        PinDischarge::check(false);
    }

    battery_manager.tick(discharge_high + 0.01, 0);
    PinDischarge::check(true);

    for (float i = discharge_high; i >= discharge_low; i -= 1) {
        battery_manager.tick(i, 0);
        PinDischarge::check(true);
    }

    battery_manager.tick(discharge_low - 0.1, 0);
    PinDischarge::check(false);

    battery_manager.tick(discharge_mid, 0);
    PinDischarge::check(false);

    battery_manager.tick(discharge_high + 0.1, 0);
    PinDischarge::check(true);

    battery_manager.tick(discharge_mid, 0);
    PinDischarge::check(true);
}

constexpr static float discharge_temp_low  = 55;
constexpr static float discharge_temp_high = 58;

void test_BatteryManager_discharging_temperature() {
    for (float t = 0; t < discharge_temp_high; t += 1) {
        battery_manager.tick(10, t);
        PinDischarge::check(true);
    }
    for (float t = discharge_temp_high + 0.1; t >= discharge_temp_low; t -= 0.1) {
        battery_manager.tick(10, t);
        PinDischarge::check(false);
    }
    for (float t = discharge_temp_low - 0.1; t <= discharge_temp_high; t += 0.1) {
        battery_manager.tick(10, t);
        PinDischarge::check(true);
    }
    for (float t = discharge_temp_high + 0.1; t >= discharge_temp_low; t -= 0.1) {
        battery_manager.tick(10, t);
        PinDischarge::check(false);
    }

    battery_manager.tick(10, discharge_temp_low - 0.1);
    PinDischarge::check(true);
}

constexpr static float heater_temp_high = 5;
constexpr static float heater_temp_low  = 0;
void test_BatteryManager_heater() {
    for (float t = -50; t < heater_temp_high; t += 1) {
        battery_manager.tick(0, t);
        PinHeater::check(true);
    }
    for (float t = heater_temp_high + 0.1; t >= heater_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        PinHeater::check(false);
    }
    for (float t = heater_temp_low - 0.1; t <= heater_temp_high; t += 1) {
        battery_manager.tick(0, t);
        PinHeater::check(true);
    }
    for (float t = heater_temp_high + 0.1; t >= heater_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        PinHeater::check(false);
    }

    battery_manager.tick(0, heater_temp_low - 0.1);
    PinHeater::check(true);
}


void test_BatteryManager() {
    UnityBegin("");

    RUN_TEST(test_BatteryManager_default_state);
    RUN_TEST(test_BatteryManager_charging_voltage);
    RUN_TEST(test_BatteryManager_charging_temperature);
    RUN_TEST(test_BatteryManager_discharging_voltage);
    RUN_TEST(test_BatteryManager_discharging_temperature);
    RUN_TEST(test_BatteryManager_heater);

    UnityEnd();
}
