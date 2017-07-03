#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_

#include <cstdint>
#include <gsl/gsl>
#include "telemetry/telemetry.h"

namespace suns {
namespace hardware {

struct Interface {
    virtual void init() = 0;
    virtual void als_measure(uint8_t gain, uint8_t itime, std::uint32_t& status, suns::Telemetry::LightData& vl, suns::Telemetry::LightData& ir) = 0;
    virtual void temperatures_measure(suns::Telemetry::Temperatures& temperature_data) = 0;
    virtual void watchdog_kick()       = 0;
    virtual void obc_interrupt_set()   = 0;
    virtual void obc_interrupt_reset() = 0;
};

using HardwareProvider = Interface*;

}  // namespace hardware
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_INTERFACE_H_
