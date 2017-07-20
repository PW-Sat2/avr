#include <assert.h>
#include <avr/sleep.h>

#include "logger.h"

#include "hardware/interface.h"
#include "hardware/mock.h"
#include "hardware/real.h"

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "commands/HouseKeeping.h"
#include "commands/PT1000.h"
#include "commands/Photodiodes.h"
#include "commands/RadFET.h"
#include "commands/SunSref.h"
#include "debug/commands.h"

using namespace hal;
using namespace hal::libs;

pld::hardware::Mock mock_hardware;
pld::hardware::RealHardware real_hardware;
pld::hardware::Interface* hw;

pld::Telemetry telemetry;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        hw->obc_interrupt_set();
        cmd.invoke(telemetry, *hw, args);
    }
};

CommandDispatcher<Executor,
                  pld::commands::SunSRef,
                  pld::commands::RadFET_On,
                  pld::commands::RadFET_Off,
                  pld::commands::RadFET_Measure,
                  pld::commands::PT1000,
                  pld::commands::Photodiodes,
                  pld::commands::HouseKeeping>
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

    Serial1.init(38400);
    Serial1.redirect_stdio();
    Serial1.redirect_stderr();
    Serial1.enable_rx_interrupt();

    real_hardware.init();
    mock_hardware.init();

    pld::debug::init();

    hw = &real_hardware;

    telemetry.init();
    telemetry.who_am_i = 0x53;

    Obc::init(&telemetry);

    sei();
    LOG_INFO("PLD Initialised.");


    while (1) {
        dispatcher.dispatch();
        hw->obc_interrupt_reset();

        hw->external_watchdog_kick();

        if (pld::debug::mock()) {
            hw = &mock_hardware;
        } else {
            hw = &real_hardware;
        }
    }

    hal::libs::sim::stop_simulation();
}
