#ifndef EPS_A_SRC_EPS_MAIN_H_
#define EPS_A_SRC_EPS_MAIN_H_

#include "IOMap.h"
#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"
#include "telemetry.h"

#include "commands/LCL.h"
#include "commands/PowerCycle.h"
#include "commands/ThermalKnives.h"

#include "LclCommander.h"
#include "TelemetryUpdater.h"
#include "ThermalKnives.h"

extern eps_a::Telemetry telemetry;

using LclCommander = eps::LclCommander<eps_a::iomap::lcl::AllLcls>;

using ThermalKnives =
    eps_a::ThermalKnives<eps_a::iomap::thermal_knives::PinSail,   //
                         eps_a::iomap::thermal_knives::PinSads>;  //

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
};

using EPSACommandDispatcher =
    CommandDispatcher<Executor,                                      //
                      eps_a::commands::PowerCycle,                   //
                      eps_a::commands::EnableLCL<LclCommander>,      //
                      eps_a::commands::DisableLCL<LclCommander>,     //
                      eps_a::commands::ThermalKnives<ThermalKnives>  //
                      >;

extern EPSACommandDispatcher dispatcher;

static void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

using Obc = drivers::ObcInterface<0x35,                                       //
                                  CommandCallback,                            //
                                  EPSACommandDispatcher::max_command_length,  //
                                  eps_a::Telemetry>;

using TelemetryUpdater = eps_a::TelemetryUpdater<telemetry,                //
                                                 eps_a::iomap::mux::Mux,   //
                                                 hal::Analog::AnalogGPIO,  //
                                                 eps_a::iomap::Mppt>;


#endif  // EPS_A_SRC_EPS_MAIN_H_