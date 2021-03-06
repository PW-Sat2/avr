#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_

#include <hal/hal>
#include "interface.h"

namespace pld {
namespace hardware {

struct Mock : public Interface {
    void init() override;

    void read_adc(std::initializer_list<ChannelDescriptor> channels) override;

    void radfet_on() override;
    RadfetMeasurement radfet_read() override;
    void radfet_off() override;

    void external_watchdog_kick() override;

    void obc_interrupt_set() override;
    void obc_interrupt_reset() override;
};

namespace mock {

extern std::array<std::uint16_t, 20> adc_channels;

}  // namespace mock
}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
