#include <cstdint>
#include <hal/hal>

#include "hardware/mock.h"

namespace pld {
namespace hardware {

using namespace mock;

hal::libs::FIFO_data<MockEvent, 100> mock::events;
std::array<std::uint16_t, 20> mock::adc_channels;

void Mock::read_adc(std::initializer_list<ChannelDescriptor> channels) {
    for (auto ch : channels) {
        uint8_t channel_ = static_cast<uint8_t>(ch.channel);

        events.append(static_cast<MockEvent>(100 + channel_));
        *ch.data = adc_channels[channel_];
        printf("[I] read from %d: %d\n", channel_, *ch.data);
    }
}
Telemetry::Radfet Mock::read_radfet() {
    events.append(MockEvent::Radfet);
    printf("[I] reading radfet\n");
    Telemetry::Radfet rf;
    return rf;
}
void Mock::watchdog_kick() {
    events.append(MockEvent::Watchdog);
    printf("[I] watchdog\n");
}
static bool pin_int = false;
void Mock::obc_interrupt_set() {
    pin_int = true;
    events.append(MockEvent::IntSet);
    printf("[I] INT\n");
}
void Mock::obc_interrupt_reset() {
    if (pin_int) {
        events.append(MockEvent::IntReset);
        printf("[I] int\n");
        pin_int = false;
    }
}

}  // namespace hardware
}  // namespace pld
