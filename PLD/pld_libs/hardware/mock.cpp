#include "hardware/mock.h"

namespace pld {
namespace hardware {

using namespace mock;
using namespace std::chrono_literals;

std::array<std::uint16_t, 20> mock::adc_channels;
using obc_int_pin = hal::DigitalIO::GPIO<33>;
using led1        = hal::DigitalIO::GPIO<36>;

void Mock::init() {
    obc_int_pin::init(hal::DigitalIO::Mode::OUTPUT);
    led1::init(hal::DigitalIO::Mode::OUTPUT);
    this->obc_interrupt_reset();
}

void Mock::read_adc(std::initializer_list<ChannelDescriptor> channels) {
    for (auto ch : channels) {
        uint8_t channel_ = static_cast<uint8_t>(ch.channel);

        *ch.data = adc_channels[channel_];

        hal::sleep_for(10ms);
    }
}

void Mock::radfet_on() {
    _delay_ms(100);
}

Telemetry::Radfet Mock::radfet_read() {
    Telemetry::Radfet rf;
    for (uint8_t i = 0; i < 10; ++i) {
        hal::sleep_for(200ms);
        this->watchdog_kick();
    }
    rf.status = Telemetry::RadfetState::MEASUREMENT_EXECUTED;
    return rf;
}

void Mock::radfet_off() {
    _delay_ms(100);
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
