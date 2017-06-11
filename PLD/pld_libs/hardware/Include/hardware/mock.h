#ifndef PLD_LIBS_MOCK_H
#define PLD_LIBS_MOCK_H

#include "interface.h"

namespace pld {
namespace hardware {

struct Mock : public Interface {
    void init() override;
    void standby() override;
    uint16_t read_adc_and_change_channel(AnalogChannel next_channel) override;
    Telemetry::Radfet read_radfet() override;
    void watchdog_kick() override;
    void obc_interrupt_set() override;
    void obc_interrupt_reset() override;
};

}  // namespace hardware
}  // namespace pld

#endif  // PLD_LIBS_MOCK_H
