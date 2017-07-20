#include "hardware/mock.h"
#include <chrono>
#include "logger.h"

namespace pld {
namespace hardware {

using namespace mock;
using namespace std::chrono_literals;

using watchdog_pin = hal::DigitalIO::GPIO<44>;
using tps3813      = hal::devices::TPS3813<watchdog_pin, 10>;

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
    hal::sleep_for(100ms);
}

Telemetry::Radfet Mock::radfet_read() {
    Telemetry::Radfet rf;
    rf.temperature = 0xBAAAAD;
    rf.vth         = {0xDEAD78, 0xBEEFED, 0x1CF00D};

    for(int i = 0; i < 4*100; ++i) {
        hal::sleep_for(60ms);
        external_watchdog_kick();
    }

    return rf;
}

void Mock::radfet_off() {
    hal::sleep_for(100ms);
}


void Mock::external_watchdog_kick() {
    tps3813::kick();
    LOG_INFO("WDT");
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
