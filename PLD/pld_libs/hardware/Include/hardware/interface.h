#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_

#include <telemetry/telemetry.h>
#include <cstdint>
#include <gsl/gsl>

namespace pld {
namespace hardware {

enum class AnalogChannel : std::uint8_t {
    SunSRef_V0,
    SunSRef_V1,
    SunSRef_V2,
    SunSRef_V3,
    SunSRef_V4,
    TemperatureSupply,
    TemperatureXp,
    TemperatureXn,
    TemperatureYp,
    TemperatureYn,
    TemperatureSADS,
    TemperatureSail,
    TemperatureCamNadir,
    TemperatureCamWing,
    Photodiode_A,
    Photodiode_B,
    Photodiode_C,
    Photodiode_D,
    HouseKeeping_3V3d,
    HouseKeeping_3V3_OBC,
};

struct Interface {
    virtual void init() = 0;
    struct ChannelDescriptor {
        AnalogChannel channel;
        uint16_t* data;
    };
    virtual void read_adc(std::initializer_list<ChannelDescriptor> channels) = 0;

    virtual pld::Telemetry::Radfet read_radfet() = 0;

    virtual void watchdog_kick()       = 0;
    virtual void obc_interrupt_set()   = 0;
    virtual void obc_interrupt_reset() = 0;
};

using HardwareProvider = Interface*;

}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
