#include <TelemetryUpdater.h>
#include "unity.h"

using namespace eps;

using AdcCh = hal::Analog::InternalADC::Input;

Telemetry tm;

struct EpsMock {
    struct IOMap {
        struct General {
            struct Data {
                std::array<uint10_t, 5> data;

                uint10_t& get(AdcCh adc) {
                    return data[num(adc)];
                }
            };

            static Data data;


            template<AdcCh adc_input>
            struct ADCMock {
                static_assert(adc_input == AdcCh::ADC0 ||      //
                                  adc_input == AdcCh::ADC1 ||  //
                                  adc_input == AdcCh::ADC2 ||  //
                                  adc_input == AdcCh::ADC3 ||  //
                                  adc_input == AdcCh::ADC5,    //
                              "Wrong channel!");

                static uint10_t read() {
                    return data.get(adc_input);
                }
            };
        };
    };

    struct LclCommander {
        static uint8_t on_status_val;

        static uint8_t on_status() {
            return on_status_val;
        }

        static uint8_t overcurrent_status_val;

        static uint8_t overcurrent_status() {
            return overcurrent_status_val;
        }
    };
};

using General      = EpsMock::IOMap::General;
using LclCommander = EpsMock::LclCommander;

General::Data General::data;
uint8_t LclCommander::on_status_val;
uint8_t LclCommander::overcurrent_status_val;

using tm_updater = TelemetryUpdater<tm, General::ADCMock, EpsMock>;

void test_TelemetryUpdater_uptime() {
    for (int i = 0; i < 1024; ++i) {
        tm_updater::update_general();
        Telemetry::General tm = ::tm.general;

        TEST_ASSERT_EQUAL_INT(i, tm.controller_b.uptime);
    }
}

void test_TelemetryUpdater_general_internal_adc() {
    for (auto adc :
         {AdcCh::ADC0, AdcCh::ADC1, AdcCh::ADC2, AdcCh::ADC3, AdcCh::ADC5}) {
        General::data.get(adc) = num(adc);
    }

    tm_updater::update_general();

    Telemetry::General tm = ::tm.general;

    auto check = [](AdcCh adc, uint10_t value) {
        TEST_ASSERT_EQUAL_INT(num(adc), value);
    };

    check(AdcCh::ADC0, tm.controller_b.temperature);
    check(AdcCh::ADC1, tm.battery_controller.voltage);
    check(AdcCh::ADC2, tm.controller_a.supply_voltage);
    check(AdcCh::ADC3, tm.controller_b.supply_temperature);
    check(AdcCh::ADC5, tm.battery_pack.temperature);
}

void test_TelemetryUpdater_boot_counters() {
    tm_updater::update_general();
    Telemetry::General tm = ::tm.general;
    TEST_ASSERT_EQUAL_INT(0, tm.controller_b.power_cycles);
    TEST_ASSERT_EQUAL_INT(0, tm.controller_b.safety_counter);

    for (int i = 1; i <= 10; ++i) {
        eps::power_cycle_counters::bootup([]() {});

        tm_updater::update_general();
        tm = ::tm.general;
        TEST_ASSERT_EQUAL_INT(i, tm.controller_b.power_cycles);
        TEST_ASSERT_EQUAL_INT(i, tm.controller_b.safety_counter);
    }

    eps::power_cycle_counters::reset();
    tm_updater::update_general();
    tm = ::tm.general;
    TEST_ASSERT_EQUAL_INT(10, tm.controller_b.power_cycles);
    TEST_ASSERT_EQUAL_INT(0, tm.controller_b.safety_counter);

    tm_updater::update_general();
    tm = ::tm.general;
    TEST_ASSERT_EQUAL_INT(10, tm.controller_b.power_cycles);
    TEST_ASSERT_EQUAL_INT(0, tm.controller_b.safety_counter);

    eps::power_cycle_counters::bootup([]() {});
    tm_updater::update_general();
    tm = ::tm.general;
    TEST_ASSERT_EQUAL_INT(11, tm.controller_b.power_cycles);
    TEST_ASSERT_EQUAL_INT(1, tm.controller_b.safety_counter);
}

void test_TelemetryUpdater() {
    UnityBegin("");

    RUN_TEST(test_TelemetryUpdater_uptime);
    RUN_TEST(test_TelemetryUpdater_general_internal_adc);
    RUN_TEST(test_TelemetryUpdater_boot_counters);

    UnityEnd();
}
