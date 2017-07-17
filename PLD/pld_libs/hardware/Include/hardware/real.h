#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_

#include <initializer_list>
#include "interface.h"
#include "telemetry/telemetry.h"

namespace pld {
namespace hardware {

struct RealHardware : public Interface {
    void init() override;

    void read_adc(std::initializer_list<ChannelDescriptor> channels) override;

    void radfet_on() override;
    RadfetMeasurement radfet_read() override;
    void radfet_off() override;

    void external_watchdog_kick() override;

    void obc_interrupt_set() override;
    void obc_interrupt_reset() override;
};

}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_REAL_H_
