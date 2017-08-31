#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_

#include <cstdint>
#include <gsl/gsl>
#include "telemetry/telemetry.h"

namespace pld {
namespace hardware {

enum class AnalogChannel : std::uint8_t {
    SunSRef_V0 = 0,
    SunSRef_V1 = 1,
    SunSRef_V2 = 2,
    SunSRef_V3 = 3,
    SunSRef_V4 = 4,

    TemperatureSupply   = 5,
    TemperatureXp       = 6,
    TemperatureXn       = 7,
    TemperatureYp       = 8,
    TemperatureYn       = 9,
    TemperatureSADS     = 10,
    TemperatureSail     = 11,
    TemperatureCamNadir = 12,
    TemperatureCamWing  = 13,

    PhotodiodeXp = 14,
    PhotodiodeXn = 15,
    PhotodiodeYp = 16,
    PhotodiodeYn = 17,

    HouseKeeping_3V3d    = 18,
    HouseKeeping_3V3_OBC = 19,

    LengthOfEnum = 20,
};

struct Interface {
    struct ChannelDescriptor {
        AnalogChannel channel;
        uint16_t* data;
    };

    struct RadfetMeasurement {
        bool timeout_vth0;
        bool timeout_vth1;
        bool timeout_vth2;
        bool timeout_temperature;
        bool overcurrent_3v3;
        bool overcurrent_5v;
        pld::Telemetry::Radfet::Measurement measurement;

        RadfetMeasurement()
            : timeout_vth0{false},
              timeout_vth1{false},
              timeout_vth2{false},
              timeout_temperature{false},
              overcurrent_3v3{false},
              overcurrent_5v{false} {
        }
    };

    virtual void init() = 0;

    virtual void read_adc(std::initializer_list<ChannelDescriptor> channels) = 0;

    virtual void radfet_on()                = 0;
    virtual RadfetMeasurement radfet_read() = 0;
    virtual void radfet_off()               = 0;

    virtual void external_watchdog_kick() = 0;

    virtual void obc_interrupt_set()   = 0;
    virtual void obc_interrupt_reset() = 0;
};

using HardwareProvider = Interface*;

}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
