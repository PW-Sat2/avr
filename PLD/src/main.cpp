#include <assert.h>

#include <avr/sleep.h>
#include <hardware/interface.h>
#include <hardware/mock.h>
#include <hal/hal>

#include <ObcInterface/CommandDispatcher.h>
#include <ObcInterface/ObcInterface.h>

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
    Serial0.init(9600);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    hw = &empty_hardware;
    std::memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    obc.init(&telemetry);
    sei();

    while (1) {
        dispatcher.dispatch();
    }

    cli();
    sleep_mode();
}
