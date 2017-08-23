#ifndef LIBS_EPS_COMMANDS_COMMANDS_OBCWATCHDOG_H_
#define LIBS_EPS_COMMANDS_COMMANDS_OBCWATCHDOG_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

template<typename ObcWatchdogModule>
struct ObcWatchdog : public Command<0xE5, 0> {
    void invoke(gsl::span<const std::uint8_t>) {
        ObcWatchdogModule::kick();
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_OBCWATCHDOG_H_
