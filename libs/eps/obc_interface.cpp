#include "defs.h"

#include "commands/DisableOverheatProtection.h"
#include "commands/LCL.h"
#include "commands/ObcWatchdog.h"
#include "commands/PowerCycle.h"
#include "commands/ThermalKnives.h"

#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

using namespace avr;

struct Executor {
    template<typename Command>
    void invoke(Command& cmd, gsl::span<uint8_t> args) {
        cmd.invoke(args);
    }
};

using EPSACommandDispatcher =
    CommandDispatcher<Executor,
                      eps::commands::PowerCycle<FullPowerCycle>,
                      eps::commands::EnableLCL<Eps::LclCommander>,
                      eps::commands::DisableLCL<Eps::LclCommander>,
                      eps::commands::ThermalKnives<ThermalKnives>,
                      eps::commands::ObcWatchdog<ObcWatchdog>,
                      eps::commands::DisableOverheatProtection<OverheatProtection>>;

EPSACommandDispatcher dispatcher;

void CommandCallback(gsl::span<const uint8_t> _c) {
    dispatcher.parse(_c);
}

using Obc = drivers::ObcInterface<ControllerSpecialisation::i2c_address,      //
                                  CommandCallback,                            //
                                  EPSACommandDispatcher::max_command_length,  //
                                  eps::Telemetry>;


ISR(TWI_vect) {
    Obc::process_interrupt();
    dispatcher.dispatch();
}

void init_obc_interface() {
    Eps::telemetry.init();
    Eps::telemetry.who_am_i = ControllerSpecialisation::who_am_i;
    Obc::init(&Eps::telemetry);
}
