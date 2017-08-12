#ifndef EPS_A_SRC_EPS_MAIN_H_
#define EPS_A_SRC_EPS_MAIN_H_

#include "IOMap.h"
#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"
#include "telemetry.h"

#include "commands/DisableOverheatProtection.h"
#include "commands/LCL.h"
#include "commands/ObcWatchdog.h"
#include "commands/PowerCycle.h"
#include "commands/ThermalKnives.h"

#include "LclCommander.h"
#include "ObcWatchdog.h"
#include "OverheatProtection.h"
#include "PowerCycle/PowerCycle.h"
#include "TelemetryUpdater.h"
#include "ThermalKnives.h"

extern eps_a::Telemetry telemetry;

using LclCommander = eps::LclCommander<eps_a::iomap::lcl::AllLcls>;
using ObcWatchdog  = eps_a::ObcWatchdog<eps_a::full_power_cycle>;


using ThermalKnives =
    eps_a::ThermalKnives<eps_a::iomap::thermal_knives::PinSail,   //
                         eps_a::iomap::thermal_knives::PinSads>;  //

using OverheatProtection = avr::OverheatProtection<LclCommander, 50>;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
};

using EPSACommandDispatcher =
    CommandDispatcher<Executor,                                       //
                      eps_a::commands::PowerCycle,                    //
                      eps_a::commands::EnableLCL<LclCommander>,       //
                      eps_a::commands::DisableLCL<LclCommander>,      //
                      eps_a::commands::ThermalKnives<ThermalKnives>,  //
                      eps_a::commands::ObcWatchdog<ObcWatchdog>,      //
                      eps_a::commands::DisableOverheatProtection<OverheatProtection>  //
                      >;

extern EPSACommandDispatcher dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c);

using Obc = drivers::ObcInterface<0x35,                                       //
                                  CommandCallback,                            //
                                  EPSACommandDispatcher::max_command_length,  //
                                  eps_a::Telemetry>;


using TelemetryUpdater =
    eps_a::TelemetryUpdater<telemetry,                //
                            eps_a::iomap::mux::Mux,   //
                            hal::Analog::AnalogGPIO,  //
                            eps_a::iomap::Mppt,       //
                            LclCommander,             //
                            eps_a::iomap::battery_controller::TemperatureSensors>;


#endif  // EPS_A_SRC_EPS_MAIN_H_
