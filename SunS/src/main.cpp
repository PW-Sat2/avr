#include <assert.h>
#include <avr/sleep.h>

#include "logger.h"

#include "hardware/interface.h"
#include "hardware/real.h"

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "commands/measure.h"

using namespace hal;
using namespace hal::libs;

suns::hardware::RealHardware real_hardware;
suns::hardware::Interface* hw;

suns::Telemetry telemetry;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        hw->obc_interrupt_set();
        cmd.invoke(telemetry, *hw, args);
    }
};

CommandDispatcher<Executor, suns::commands::Measure> dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

using Obc =
    ::drivers::ObcInterface<0x44, CommandCallback, decltype(dispatcher)::max_command_length, suns::Telemetry>;

ISR(TWI_vect) {
    Obc::process_interrupt();
}

int main() {
    hal::Watchdog::enable(hal::Watchdog::Period::p8000ms);
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
    Serial0.enable_rx_interrupt();
    LOG_INFO("SunS Start.");

    real_hardware.init();
    hw = &real_hardware;

    telemetry.init();
    telemetry.who_am_i = 0x11;

    Obc::init(&telemetry);
    sei();
    LOG_INFO("SunS Initialised.");


    while (1) {
        dispatcher.dispatch();
        hw->watchdog_kick();
        hw->obc_interrupt_reset();
    }

    cli();
    sleep_mode();
}
