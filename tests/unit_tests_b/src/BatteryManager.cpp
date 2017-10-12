#include <hal/hal>
#include "unity.h"

#include "BatteryManager.h"

using namespace avr;

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

template<int tag>
struct HeaterMock {
    static void enable() {
        PinMock<tag>::set();
    }
    static void disable() {
        PinMock<tag>::reset();
    }
    static void check(bool desired) {
        PinMock<tag>::check(desired);
    }
    static bool heating() {
        return PinMock<tag>::read();
    }
};

using PinCharge    = PinMock<0>;
using PinDischarge = PinMock<1>;
using PinHeater    = HeaterMock<2>;

eps::BatteryManager<PinCharge, PinDischarge, PinHeater> battery_manager;

void checkCharge(bool desired) {
    PinCharge::check(desired);
    TEST_ASSERT_EQUAL(desired, battery_manager.state() & 1);
}

void checkDischarge(bool desired) {
    PinDischarge::check(desired);
    TEST_ASSERT_EQUAL(desired, (battery_manager.state() >> 1) & 1);
}

void checkHeater(bool desired) {
    PinHeater::check(desired);
    TEST_ASSERT_EQUAL(desired, (battery_manager.state() >> 2) & 1);
}

void test_BatteryManager_default_state() {
    battery_manager.tick(0, 1000);

    checkCharge(false);
}

constexpr static float charge_high = 8.0;
constexpr static float charge_low  = 7.8;
constexpr static float charge_mid  = (charge_high + charge_low) / 2.0;

void test_BatteryManager_charging_voltage() {
    for (float i = 0; i < charge_high; i += 1) {
        battery_manager.tick(i, 0);
        checkCharge(true);
        TEST_ASSERT_EQUAL(true, battery_manager.state() & 1);
    }

    battery_manager.tick(charge_high + 0.01, 0);
    checkCharge(false);

    for (float i = charge_high; i >= charge_low; i -= 1) {
        battery_manager.tick(i, 0);
        checkCharge(false);
    }

    battery_manager.tick(charge_low - 0.1, 0);
    checkCharge(true);

    battery_manager.tick(charge_mid, 0);
    checkCharge(true);

    battery_manager.tick(charge_high + 0.1, 0);
    checkCharge(false);

    battery_manager.tick(charge_mid, 0);
    checkCharge(false);
}

constexpr static float charge_temp_high = 43;
constexpr static float charge_temp_low  = 40;

void test_BatteryManager_charging_temperature() {
    for (float t = 0; t < charge_temp_high; t += 1) {
        battery_manager.tick(0, t);
        checkCharge(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        checkCharge(false);
    }
    for (float t = charge_temp_low - 0.1; t <= charge_temp_high; t += 1) {
        battery_manager.tick(0, t);
        checkCharge(true);
    }
    for (float t = charge_temp_high + 0.1; t >= charge_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        checkCharge(false);
    }

    battery_manager.tick(0, charge_temp_low - 0.1);
    checkCharge(true);
}

constexpr static float discharge_low  = 6.3;
constexpr static float discharge_high = 6.8;
constexpr static float discharge_mid  = (discharge_high + discharge_low) / 2.0;

void test_BatteryManager_discharging_voltage() {
    for (float i = 0; i < discharge_high; i += 1) {
        battery_manager.tick(i, 0);
        checkDischarge(false);
    }

    battery_manager.tick(discharge_high + 0.01, 0);
    checkDischarge(true);

    for (float i = discharge_high; i >= discharge_low; i -= 1) {
        battery_manager.tick(i, 0);
        checkDischarge(true);
    }

    battery_manager.tick(discharge_low - 0.1, 0);
    checkDischarge(false);

    battery_manager.tick(discharge_mid, 0);
    checkDischarge(false);

    battery_manager.tick(discharge_high + 0.1, 0);
    checkDischarge(true);

    battery_manager.tick(discharge_mid, 0);
    checkDischarge(true);
}

constexpr static float discharge_temp_low  = 55;
constexpr static float discharge_temp_high = 58;

void test_BatteryManager_discharging_temperature() {
    for (float t = 0; t < discharge_temp_high; t += 1) {
        battery_manager.tick(10, t);
        checkDischarge(true);
    }
    for (float t = discharge_temp_high + 0.1; t >= discharge_temp_low; t -= 0.1) {
        battery_manager.tick(10, t);
        checkDischarge(false);
    }
    for (float t = discharge_temp_low - 0.1; t <= discharge_temp_high; t += 0.1) {
        battery_manager.tick(10, t);
        checkDischarge(true);
    }
    for (float t = discharge_temp_high + 0.1; t >= discharge_temp_low; t -= 0.1) {
        battery_manager.tick(10, t);
        checkDischarge(false);
    }

    battery_manager.tick(10, discharge_temp_low - 0.1);
    checkDischarge(true);
}

constexpr static float heater_temp_high = -2;
constexpr static float heater_temp_low  = -7;

void test_BatteryManager_heater() {
    for (float t = -50; t < heater_temp_high; t += 1) {
        battery_manager.tick(0, t);
        checkHeater(true);
    }
    for (float t = heater_temp_high + 0.1; t >= heater_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        checkHeater(false);
    }
    for (float t = heater_temp_low - 0.1; t <= heater_temp_high; t += 1) {
        battery_manager.tick(0, t);
        checkHeater(true);
    }
    for (float t = heater_temp_high + 0.1; t >= heater_temp_low; t -= 1) {
        battery_manager.tick(0, t);
        checkHeater(false);
    }

    battery_manager.tick(0, heater_temp_low - 0.1);
    checkHeater(true);
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
