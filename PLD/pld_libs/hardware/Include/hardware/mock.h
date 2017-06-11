#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_

#include <hal/hal>
#include "interface.h"

namespace pld {
namespace hardware {

struct Mock : public Interface {
    virtual void init() override;
    virtual void read_adc(std::initializer_list<ChannelDescriptor> channels) override;
    Telemetry::Radfet read_radfet() override;
    void watchdog_kick() override;
    void obc_interrupt_set() override;
    void obc_interrupt_reset() override;
};

namespace mock {
enum class MockEvent : uint8_t {
    Init     = 0,
    Standby  = 1,
    Radfet   = 2,
    Watchdog = 3,
    IntSet   = 4,
    IntReset = 5,
};

extern std::array<std::uint16_t, 20> adc_channels;

}  // namespace mock
}  // namespace hardware
}  // namespace pld

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_MOCK_H_
