#ifndef EPS_A_EPS_LIBS_COMMANDS_COMMANDS_OBCWATCHDOG_H_
#define EPS_A_EPS_LIBS_COMMANDS_COMMANDS_OBCWATCHDOG_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps_a {
namespace commands {

template<typename ObcWatchdogModule>
struct ObcWatchdog : public Command<0xE5, 0> {
    void invoke(gsl::span<const std::uint8_t>) {
        ObcWatchdogModule::kick();
    }
};

}  // namespace commands
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_COMMANDS_COMMANDS_OBCWATCHDOG_H_
