#include <cstdint>
#include <hal/hal>

#include "hardware/mock.h"

namespace pld {
namespace hardware {

using namespace mock;

hal::libs::FIFO_data<MockEvent, 100> mock::events;
std::array<std::uint16_t, 20> mock::adc_channels;

void Mock::init() {
    events.append(MockEvent::Init);
    printf("[I] init\n");
}
void Mock::standby() {
    events.append(MockEvent::Standby);
    printf("[I] standby\n");
}
uint16_t Mock::read_adc_and_change_channel(AnalogChannel channel) {
    static uint8_t channel_ = 0;

    events.append(static_cast<MockEvent>(100 + channel_));
    auto value = adc_channels[channel_];
    printf("[I] read from %d: %d\n", channel_, value);

    channel_ = static_cast<uint8_t>(channel);

    return value;
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
void Mock::obc_interrupt_set() {
    events.append(MockEvent::IntSet);
    printf("[I] INT\n");
}
void Mock::obc_interrupt_reset() {
    events.append(MockEvent::IntReset);
    printf("[I] int\n");
}

}  // namespace hardware
}  // namespace pld
