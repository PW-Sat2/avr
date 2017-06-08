#define __ASSERT_USE_STDERR
#include <assert.h>

#include <hal/hal>
#include "hw.h"
#include "telecommands/HouseKeeping.h"
#include "telecommands/PT1000.h"
#include "telecommands/Photodiodes.h"
#include "telecommands/RadFET.h"
#include "telecommands/SunSref.h"
#include "telemetry.h"

using namespace hal;
using namespace hal::libs;


PLDHardware hw;

PLD::telecommands::SunSRef sunS;
PLD::telecommands::RadFET radFET;
PLD::telecommands::PT1000 pt1000;
PLD::telecommands::Photodiodes photodiodes;
PLD::telecommands::HouseKeeping houseKeeping;

using CommandDispatcherType = CommandDispatcher<PLD::telecommands::SunSRef,
                                                PLD::telecommands::RadFET,
                                                PLD::telecommands::PT1000,
                                                PLD::telecommands::Photodiodes,
                                                PLD::telecommands::HouseKeeping>;


CommandDispatcherType
    dispatcher({&sunS, &radFET, &pt1000, &photodiodes, &houseKeeping});

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

::drivers::ObcInterface<0x1A, CommandCallback, CommandDispatcherType::max_command_length, PLD::Telemetry>
    obc;

ISR(TWI_vect) {
    obc.process_interrupt();
}

TelemetryBuffer<PLD::Telemetry> telemetryBuffer(obc.set_memory);

int main() {
    Serial0.init(115200);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
    printf("AAA\n");

    hw.init();

    for (auto cmd = 0x80; cmd <= 0x84; ++cmd) {
        std::array<uint8_t, 1> _d;
        _d[0] = cmd;
        dispatcher.parse(_d);
        dispatcher.dispatch();
    }


    ////    PLD::telemetry.radfet.temperature = 0x1234;
    //
    ////    obc.init(&PLD::telemetry);
    //    sei();
    //    while (1) {
    //        dispatcher.dispatch();
    //    }
}
