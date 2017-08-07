#ifndef EPS_A_EPS_LIBS_TELEMETRY_TELEMETRY_H_
#define EPS_A_EPS_LIBS_TELEMETRY_TELEMETRY_H_

#include <cstring>
#include "atomic.h"

namespace eps_a {

struct Telemetry {
    struct SingleMpptChannelState {
        bool mppt_achieved;
    };

    struct SingleMpptChannel {
        std::uint16_t solar_current;
        std::uint16_t solar_voltage;
        std::uint16_t output_voltage;
        std::uint16_t temperature;
        SingleMpptChannelState state;
    };

    struct Distribution {
        std::uint16_t dcdc_3v3_current;
        std::uint16_t dcdc_3v3_voltage;
        std::uint16_t dcdc_5v_current;
        std::uint16_t dcdc_5v_voltage;
        std::uint16_t vbat_current;
        std::uint16_t vbat_voltage;
        std::uint8_t lcl_state;
        std::uint8_t lcl_flagb;
    };

    struct BatteryController {
        std::uint16_t controller_a_voltage;
        std::uint16_t charge_current;
        std::uint16_t discharge_current;
        std::uint16_t temperature;
        std::uint8_t state;
    };

    struct BatteryPack {
        std::uint16_t temperature_a;
        std::uint16_t temperature_b;
    };

    struct ControllerB {
        std::uint16_t supply_voltage;
    };

    struct ControllerA {
        std::uint8_t safety_counter;
        std::uint16_t power_cycles;
        std::uint32_t uptime;
        std::uint16_t temperature;
        std::uint16_t supply_temperature;
    };

    struct AllMpptChannels {
        SingleMpptChannel mpptx;
        SingleMpptChannel mpptyp;
        SingleMpptChannel mpptyn;
    };

    struct General {
        Distribution distribution;
        BatteryController battery_controller;
        BatteryPack battery_pack;
        ControllerB controller_b;
        ControllerA controller_a;
        std::uint16_t dcdc_3v3_temperature;
        std::uint16_t dcdc_5v_temperature;
    };

    std::uint8_t who_am_i;
    avr::Atomic<AllMpptChannels> mppt;
    avr::Atomic<General> general;

    void init() {
        std::memset(this, 0xFF, sizeof(eps_a::Telemetry));
    }
};
static_assert(sizeof(Telemetry) == 72,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_TELEMETRY_TELEMETRY_H_
