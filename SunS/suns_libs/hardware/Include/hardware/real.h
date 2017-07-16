#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_

#include <initializer_list>
#include "interface.h"
#include "telemetry/telemetry.h"

namespace suns {
namespace hardware {

struct RealHardware : public Interface {
    virtual void init() override;
    virtual void als_measure(uint8_t gain,
                             uint8_t itime,
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

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
