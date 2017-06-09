#ifndef PLD_LIBS_MOCK_H
#define PLD_LIBS_MOCK_H

#include <hal/hal>
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

namespace mock {
enum class MockEvent : uint8_t {
    Init = 0,
    Standby = 1,
    Radfet = 2,
    Watchdog = 3,
    IntSet = 4,
    IntReset = 5,
};

extern std::array<std::uint16_t, 20> adc_channels;
extern hal::libs::FIFO_data<MockEvent, 100> events;

}
}  // namespace hardware
}  // namespace pld

#endif  // PLD_LIBS_MOCK_H
