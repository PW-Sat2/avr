#include <assert.h>
#include <avr/sleep.h>

#include "logger.h"

#include "hardware/interface.h"
#include "hardware/mock.h"

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "debug/commands.h"
#include "telecommands/HouseKeeping.h"
#include "telecommands/PT1000.h"
#include "telecommands/Photodiodes.h"
#include "telecommands/RadFET.h"
#include "telecommands/SunSref.h"

using namespace hal;
using namespace hal::libs;

pld::hardware::Mock empty_hardware;
pld::hardware::Interface* hw;

pld::Telemetry telemetry;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(telemetry, *hw, args);
    }
};

CommandDispatcher<Executor,
                  pld::telecommands::SunSRef,
                  pld::telecommands::RadFET,
                  pld::telecommands::PT1000,
                  pld::telecommands::Photodiodes,
                  pld::telecommands::HouseKeeping>
    dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

using Obc =
    ::drivers::ObcInterface<0x30, CommandCallback, decltype(dispatcher)::max_command_length, pld::Telemetry>;

ISR(TWI_vect) {
    Obc::process_interrupt();
}

int main() {
    hal::Watchdog::disable();
    empty_hardware.init();

    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
    Serial0.enable_rx_interrupt();

    pld::debug::init();

    hw = &empty_hardware;
    telemetry.init();
    telemetry.who_am_i = 0x53;

    Obc::init(&telemetry);
    sei();
    LOG_INFO("PLD Initialised.");

    hal::Watchdog::enable(hal::Watchdog::Period::p500ms);

    while (1) {
        dispatcher.dispatch();
        hw->watchdog_kick();
        hw->obc_interrupt_reset();
    }

    cli();
    sleep_mode();
}
