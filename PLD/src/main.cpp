#include <assert.h>
#include <avr/sleep.h>

#include <hardware/interface.h>
#include <hardware/mock.h>

#include <ObcInterface/CommandDispatcher.h>
#include <ObcInterface/ObcInterface.h>

#include "debug/commands.h"
#include "telecommands/HouseKeeping.h"
#include "telecommands/PT1000.h"
#include "telecommands/Photodiodes.h"
#include "telecommands/RadFET.h"
#include "telecommands/SunSref.h"


using namespace hal;
using namespace hal::libs;

pld::hardware::Mock empty_hardware;
pld::hardware::HardwareProvider hw;

pld::Telemetry telemetry;

using CommandDispatcherType = CommandDispatcher<pld::telecommands::SunSRef,
                                                pld::telecommands::RadFET,
                                                pld::telecommands::PT1000,
                                                pld::telecommands::Photodiodes,
                                                pld::telecommands::HouseKeeping>;

pld::telecommands::SunSRef sunS(telemetry, hw);
pld::telecommands::RadFET radFET(telemetry, hw);
pld::telecommands::PT1000 pt1000(telemetry, hw);
pld::telecommands::Photodiodes photodiodes(telemetry, hw);
pld::telecommands::HouseKeeping houseKeeping(telemetry, hw);

CommandDispatcherType
    dispatcher({&sunS, &radFET, &pt1000, &photodiodes, &houseKeeping});

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

::drivers::ObcInterface<0x1A, CommandCallback, CommandDispatcherType::max_command_length, pld::Telemetry>
    obc;


ISR(TWI_vect) {
    obc.process_interrupt();
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
    std::memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    obc.init(&telemetry);
    sei();
    printf("PLD Initialised.\n");

    hal::Watchdog::enable(hal::Watchdog::Period::p500ms);

    while (1) {
        dispatcher.dispatch();
        hw->watchdog_kick();
        hw->obc_interrupt_reset();
    }

    cli();
    sleep_mode();
}
