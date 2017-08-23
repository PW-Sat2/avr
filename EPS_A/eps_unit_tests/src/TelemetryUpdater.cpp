#include <TelemetryUpdater.h>
#include "unity.h"

using namespace eps;

using MuxCh = hal::devices::ADG709::Channel;
using AdcCh = hal::Analog::InternalADC::Input;

Telemetry tm;

struct EpsMock {
    struct IOMap {
        struct General {
            struct Data {
                std::array<std::array<uint10_t, 4>, 4> data;
                MuxCh selected_mux_channel;

                uint10_t& get(MuxCh mux, AdcCh adc) {
                    return data[num(adc)][num(mux)];
                }

                static uint10_t sim(MuxCh mux, AdcCh ch) {
                    uint16_t val = 10u * num(mux) + num(ch);
                    return val;
                }
            };

            static Data data;


            template<AdcCh adc_input>
            struct ADCMock {
                static_assert(adc_input == AdcCh::ADC0 ||      //
                                  adc_input == AdcCh::ADC1 ||  //
                                  adc_input == AdcCh::ADC2 ||  //
                                  adc_input == AdcCh::ADC3,
                              "Wrong channel!");

                static uint10_t read() {
                    return data.get(data.selected_mux_channel, adc_input);
                }
            };
        };

        struct Mux {
            static void select(MuxCh ch) {
                General::data.selected_mux_channel = ch;
            }
        };


        struct Mppt {
            using Adc124Ch = hal::devices::ADC124::Channel;

            enum class MpptChannel {
                X  = 0,
                Yp = 1,
                Yn = 2,
            };

            struct Data {
                std::array<bool, 3> initialised;
                std::array<std::array<uint12_t, 4>, 3> data;

                uint12_t& get(MpptChannel ch, Adc124Ch adc) {
                    return data[num(ch)][num(adc) >> 3];
                }

                uint12_t sim(MpptChannel ch, Adc124Ch adc) {
                    uint12_t val = 10u * num(ch) + (num(adc) >> 3);
                    return val;
                }
            };

            static Data data;

            template<MpptChannel mppt_channel>
            struct MpptChannelMock {
                struct AdcSpi {
                    static void init() {
                        TEST_ASSERT_FALSE(data.initialised[num(mppt_channel)]);
                        data.initialised[num(mppt_channel)] = true;
                    }
                };

                struct Adc124 {
                    static uint12_t read_and_change_channel(Adc124Ch ch) {
                        static Adc124Ch channel = Adc124Ch::IN0;

                        TEST_ASSERT_TRUE(data.initialised[num(mppt_channel)]);

                        uint12_t val = data.get(mppt_channel, channel);

                        channel = ch;
                        return val;
                    }
                };
            };
            using MpptX  = MpptChannelMock<MpptChannel::X>;
            using MpptYp = MpptChannelMock<MpptChannel::Yp>;
            using MpptYn = MpptChannelMock<MpptChannel::Yn>;
        };

        struct battery_controller {
            template<int tag>
            struct TempSensorMock {
                static bool initialised;

                struct Spi {
                    static void init() {
                        initialised = true;
                    }
                };

                struct Tmp121 {
                    static uint13_t value;

                    static uint13_t read_raw() {
                        if (!initialised)
                            return 0;

                        initialised = false;
                        return value;
                    }
                };
            };


            struct TemperatureSensors {
                using TemperatureSensorA = TempSensorMock<0>;
                using TemperatureSensorB = TempSensorMock<1>;
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

using General         = EpsMock::IOMap::General;
using Mppt            = EpsMock::IOMap::Mppt;
using LclCommander    = EpsMock::LclCommander;
using TempSensorsMock = EpsMock::IOMap::battery_controller::TemperatureSensors;
template<int tag>
using TempSensorMock = EpsMock::IOMap::battery_controller::TempSensorMock<tag>;

General::Data General::data;
Mppt::Data Mppt::data;
uint8_t LclCommander::on_status_val;
uint8_t LclCommander::overcurrent_status_val;

template<int tag>
bool EpsMock::IOMap::battery_controller::TempSensorMock<tag>::initialised;
template<int tag>
uint13_t EpsMock::IOMap::battery_controller::TempSensorMock<tag>::Tmp121::value;

// using tm_updater =
//    TelemetryUpdater<tm, General::Mux, General::ADCMock, Mppt::MpptMock,
//    LclCommander, TempSensorsMock>;
using tm_updater = TelemetryUpdater<tm, General::ADCMock, EpsMock>;

void test_TelemetryUpdater_general() {
    for (auto mux : {MuxCh::S1, MuxCh::S2, MuxCh::S3, MuxCh::S4}) {
        for (auto adc : {AdcCh::ADC0, AdcCh::ADC1, AdcCh::ADC2, AdcCh::ADC3}) {
            General::data.get(mux, adc) = General::Data::sim(mux, adc);
        }
    }

    tm_updater::update_general();

    Telemetry::General tm = ::tm.general;

    auto check = [](MuxCh mux, AdcCh adc, uint10_t value) {
        TEST_ASSERT_EQUAL_INT(General::Data::sim(mux, adc), value);
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

void test_TelemetryUpdater_mppt() {
    Mppt::data.initialised.fill(false);

    for (auto ch :
         {Mppt::MpptChannel::X, Mppt::MpptChannel::Yp, Mppt::MpptChannel::Yn}) {
        for (auto adc : {Mppt::Adc124Ch::IN0,
                         Mppt::Adc124Ch::IN1,
                         Mppt::Adc124Ch::IN2,
                         Mppt::Adc124Ch::IN3}) {
            Mppt::data.get(ch, adc) = Mppt::data.sim(ch, adc);
        }
    }

    tm_updater::update_mppt();

    Telemetry::AllMpptChannels tm = ::tm.mppt;

    auto check = [=](Mppt::Adc124Ch adc,
                     uint16_t Telemetry::SingleMpptChannel::*ptr) {
        TEST_ASSERT_EQUAL_INT(Mppt::data.sim(Mppt::MpptChannel::X, adc),
                              tm.mpptx.*ptr);
        TEST_ASSERT_EQUAL_INT(Mppt::data.sim(Mppt::MpptChannel::Yp, adc),
                              tm.mpptyp.*ptr);
        TEST_ASSERT_EQUAL_INT(Mppt::data.sim(Mppt::MpptChannel::Yn, adc),
                              tm.mpptyn.*ptr);
    };

    check(Mppt::Adc124Ch::IN0, &Telemetry::SingleMpptChannel::solar_current);
    check(Mppt::Adc124Ch::IN1, &Telemetry::SingleMpptChannel::solar_voltage);
    check(Mppt::Adc124Ch::IN2, &Telemetry::SingleMpptChannel::output_voltage);
    check(Mppt::Adc124Ch::IN3, &Telemetry::SingleMpptChannel::temperature);
}

void test_TelemetryUpdater_lcl_status() {
    LclCommander::overcurrent_status_val = 0xA7;

    uint8_t val = 0;
    do {
        LclCommander::on_status_val = val;

        tm_updater::update_general();

        Telemetry::General gen = tm.general;
        TEST_ASSERT_EQUAL(0xA7, gen.distribution.lcl_flagb);
        TEST_ASSERT_EQUAL(val, gen.distribution.lcl_state);
    } while (val++ != 0xFF);


    LclCommander::on_status_val = 0x5E;

    val = 0;
    do {
        LclCommander::overcurrent_status_val = val;

        tm_updater::update_general();

        Telemetry::General gen = tm.general;
        TEST_ASSERT_EQUAL(0x5E, gen.distribution.lcl_state);
        TEST_ASSERT_EQUAL(val, gen.distribution.lcl_flagb);
    } while (val++ != 0xFF);
}

void test_TelemetryUpdater_battery_pack_temperature_sensors() {
    TempSensorsMock::TemperatureSensorB::Tmp121::value = 0x1FEF;

    uint16_t val = 0;
    do {
        TempSensorsMock::TemperatureSensorA::Tmp121::value = val;

        tm_updater::update_general();

        Telemetry::General gen = tm.general;
        TEST_ASSERT_EQUAL(val, gen.battery_pack.temperature_a);
        TEST_ASSERT_EQUAL(0x1FEF, gen.battery_pack.temperature_b);
    } while (val++ != 0x1FFF);


    TempSensorsMock::TemperatureSensorA::Tmp121::value = 0x1ABC;

    val = 0;
    do {
        TempSensorsMock::TemperatureSensorB::Tmp121::value = val;

        tm_updater::update_general();

        Telemetry::General gen = tm.general;
        TEST_ASSERT_EQUAL(0x1ABC, gen.battery_pack.temperature_a);
        TEST_ASSERT_EQUAL(val, gen.battery_pack.temperature_b);
    } while (val++ != 0x1FFF);
}

void test_TelemetryUpdater() {
    UnityBegin("");

    RUN_TEST(test_TelemetryUpdater_general);
    RUN_TEST(test_TelemetryUpdater_mppt);
    RUN_TEST(test_TelemetryUpdater_lcl_status);
    RUN_TEST(test_TelemetryUpdater_battery_pack_temperature_sensors);

    UnityEnd();
}
