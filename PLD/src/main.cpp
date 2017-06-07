#define __ASSERT_USE_STDERR
#include <assert.h>

#include <ObcInterface/ObcInterface.h>
#include <hal/board.h>
#include <hal/hal>
#include "telecommands/RadFET.h"
#include "telecommands/SunSref.h"
#include "telemetry/telemetry.h"

using namespace hal;
using namespace hal::libs;


PLD::telecommands::SunSRef sunS;
PLD::telecommands::RadFET radFET;

using CommandDispatcherType =
    CommandDispatcher<PLD::telecommands::SunSRef, PLD::telecommands::RadFET>;


CommandDispatcherType dispatcher({&sunS, &radFET});

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

::drivers::ObcInterface<0x1A, CommandCallback, CommandDispatcherType::max_command_length, PLD::Telemetry>
    obc;

ISR(TWI_vect) {
    obc.process_interrupt();
}

int main() {
    Serial0.init(115200);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    PLD::telemetry.radfet.temperature = 0x1234;

    obc.init(&PLD::telemetry);
    sei();
    while (1) {
        dispatcher.dispatch();
    }
}
