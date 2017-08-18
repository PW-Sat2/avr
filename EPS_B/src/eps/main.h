#ifndef EPS_B_SRC_EPS_MAIN_H_
#define EPS_B_SRC_EPS_MAIN_H_

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "commands/PowerCycle.h"

#include "telemetry.h"

extern eps_b::Telemetry telemetry;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
};

using EPSBCommandDispatcher =
    CommandDispatcher<Executor, eps_b::commands::PowerCycle>;

extern EPSBCommandDispatcher dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c);

using Obc = drivers::ObcInterface<0x36,                                       //
                                  CommandCallback,                            //
                                  EPSBCommandDispatcher::max_command_length,  //
                                  eps_b::Telemetry>;

#endif  // EPS_B_SRC_EPS_MAIN_H_
