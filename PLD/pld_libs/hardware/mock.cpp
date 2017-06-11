#include <cstdint>
#include <hal/hal>

#include "hardware/mock.h"

namespace pld {
namespace hardware {

void Mock::init() {
    printf("[I] init\n");
}
void Mock::standby() {
    printf("[I] standby\n");
}
uint16_t Mock::read_adc_and_change_channel(AnalogChannel channel) {
    printf("[I] read from %d\n", static_cast<uint8_t>(channel));
    return 0;
}
Telemetry::Radfet Mock::read_radfet() {
    printf("[I] reading radfet\n");
    Telemetry::Radfet rf;
    return rf;
}
void Mock::watchdog_kick() {
    printf("[I] watchdog\n");
}
void Mock::obc_interrupt_set() {
    printf("[I] INT\n");
}
void Mock::obc_interrupt_reset() {
    printf("[I] int\n");
}
}  // namespace hardware
}  // namespace pld
