#ifndef SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
#define SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_

#include <initializer_list>
#include "interface.h"
#include "telemetry/telemetry.h"

namespace suns {
namespace hardware {

struct RealHardware : public Interface {
    virtual void init() override;
    virtual void als_measure(suns::Telemetry::Params parameters,
                             suns::Telemetry::Status& als_status,
                             suns::Telemetry::LightData& vl,
                             suns::Telemetry::LightData& ir) override;
    virtual void
    temperatures_measure(suns::Telemetry::Temperatures& temperature_data) override;
    virtual void watchdog_kick() override;
    virtual void obc_interrupt_set() override;
    virtual void obc_interrupt_reset() override;
};

}  // namespace hardware
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
