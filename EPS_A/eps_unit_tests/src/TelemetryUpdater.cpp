#include <TelemetryUpdater.h>
#include "unity.h"

using namespace eps_a;

using MuxCh = hal::devices::ADG709::Channel;
using AdcCh = hal::Analog::InternalADC::Input;

Telemetry tm;

struct Data {
    std::array<std::array<uint10_t, 4>, 4> data;

    uint10_t& get(MuxCh mux, AdcCh adc) {
        return data[num(adc)][num(mux)];
    }
};

Data data;

MuxCh selected_mux_channel;

struct MuxMock {
    static void select(MuxCh ch) {
        selected_mux_channel = ch;
    }
};


template<AdcCh adc_input>
struct ADCMock {
    static_assert(adc_input == AdcCh::ADC0 ||      //
                      adc_input == AdcCh::ADC1 ||  //
                      adc_input == AdcCh::ADC2 ||  //
                      adc_input == AdcCh::ADC3,
                  "Wrong channel!");

    static uint10_t read() {
        return data.get(selected_mux_channel, adc_input);
    }
};

using tm_updater = TelemetryUpdater<tm, MuxMock, ADCMock>;

uint10_t count(MuxCh mux, AdcCh ch) {
    uint16_t val = 10u * num(mux) + num(ch);
    return val;
}

void test_TelemetryUpdater_example_pass() {
    for (auto mux : {MuxCh::S1, MuxCh::S2, MuxCh::S3, MuxCh::S4}) {
        for (auto adc : {AdcCh::ADC0, AdcCh::ADC1, AdcCh::ADC2, AdcCh::ADC3}) {
            data.get(mux, adc) = count(mux, adc);
        }
    }

    tm_updater::update_general();

    Telemetry::General tm = ::tm.general;

    auto check = [](MuxCh mux, AdcCh adc, uint10_t value) {
        TEST_ASSERT_EQUAL_INT(count(mux, adc), value);
    };

    check(MuxCh::S1, AdcCh::ADC0, tm.distribution.vbat_current);
    check(MuxCh::S1, AdcCh::ADC1, tm.distribution.dcdc_5v_current);
    check(MuxCh::S1, AdcCh::ADC3, tm.battery_controller.temperature);

    check(MuxCh::S2, AdcCh::ADC0, tm.distribution.vbat_voltage);
    check(MuxCh::S2, AdcCh::ADC1, tm.distribution.dcdc_5v_voltage);
    check(MuxCh::S2, AdcCh::ADC2, tm.controller_a.temperature);
    check(MuxCh::S2, AdcCh::ADC3, tm.battery_controller.charge_current);

    check(MuxCh::S3, AdcCh::ADC0, tm.distribution.dcdc_3v3_current);
    check(MuxCh::S3, AdcCh::ADC1, tm.dcdc_3v3_temperature);
    check(MuxCh::S3, AdcCh::ADC2, tm.controller_b.supply_voltage);
    check(MuxCh::S3, AdcCh::ADC3, tm.battery_controller.discharge_current);

    check(MuxCh::S4, AdcCh::ADC0, tm.distribution.dcdc_3v3_voltage);
    check(MuxCh::S4, AdcCh::ADC1, tm.dcdc_5v_temperature);
    check(MuxCh::S4, AdcCh::ADC2, tm.controller_a.supply_temperature);
    check(MuxCh::S4, AdcCh::ADC3, tm.battery_controller.controller_a_voltage);
}

void test_TelemetryUpdater() {
    UnityBegin("");

    RUN_TEST(test_TelemetryUpdater_example_pass);

    UnityEnd();
}
