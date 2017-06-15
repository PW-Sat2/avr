#include "hardware/mock.h"

namespace pld {
namespace hardware {

using namespace mock;

std::array<std::uint16_t, 20> mock::adc_channels;
using obc_int_pin = hal::DigitalIO::GPIO<33>;
using led1 = hal::DigitalIO::GPIO<36>;

void Mock::init() {
    obc_int_pin::init(hal::DigitalIO::Mode::OUTPUT);
    led1::init(hal::DigitalIO::Mode::OUTPUT);
    this->obc_interrupt_reset();
}

void Mock::read_adc(std::initializer_list<ChannelDescriptor> channels) {
    for (auto ch : channels) {
        uint8_t channel_ = static_cast<uint8_t>(ch.channel);

        *ch.data = adc_channels[channel_];

        _delay_ms(10);
    }
}

Telemetry::Radfet Mock::read_radfet() {
    Telemetry::Radfet rf;
    for (uint8_t i = 0; i < 10; ++i) {
        _delay_ms(200);
        this->watchdog_kick();
    }
    return rf;
}

void Mock::watchdog_kick() {
    hal::Watchdog::kick();
}

void Mock::obc_interrupt_set() {
    obc_int_pin::set();
    led1::set();
}

void Mock::obc_interrupt_reset() {
    obc_int_pin::reset();
    led1::reset();
}

}  // namespace hardware
}  // namespace pld
