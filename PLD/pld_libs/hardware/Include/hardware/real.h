#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_

#include <initializer_list>
#include "interface.h"
#include "telemetry/telemetry.h"

namespace pld {
namespace hardware {

struct RealHardware : public Interface {
    virtual void init() override;
    virtual void
    read_adc(std::initializer_list<ChannelDescriptor> channels) override;
    virtual pld::Telemetry::Radfet read_radfet() override;
    virtual void watchdog_kick() override;
    virtual void obc_interrupt_set() override;
    virtual void obc_interrupt_reset() override;
};

}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
