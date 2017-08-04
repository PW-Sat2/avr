#include <hal/hal>
#include "IOMap.h"
#include "MainTimer.h"

#include "logger.h"

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "commands/PowerCycle.h"

#include "telemetry.h"

using namespace hal;
using namespace hal::libs;

eps_a::Telemetry telemetry;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
};

using EPSACommandDispatcher = CommandDispatcher<Executor,                    //
                                                eps_a::commands::PowerCycle  //
                                                >;
EPSACommandDispatcher dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

using Obc = drivers::ObcInterface<0x35,                                       //
                                  CommandCallback,                            //
                                  EPSACommandDispatcher::max_command_length,  //
                                  eps_a::Telemetry>;

ISR(TWI_vect) {
    Obc::process_interrupt();
    dispatcher.dispatch();
}

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    telemetry.init();
    telemetry.who_am_i = 0x61;
    Obc::init(&telemetry);

    eps_a::MainTimer::init();

    sei();

    LOG_INFO("EPS A initialized.");

    while (1) {
    }

    hal::libs::sim::stop_simulation();
}
