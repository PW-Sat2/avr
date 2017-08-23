#ifndef LIBS_EPS_COMMANDS_COMMANDS_POWERCYCLE_H_
#define LIBS_EPS_COMMANDS_COMMANDS_POWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

template<typename FullPowerCycle>
class PowerCycle : public Command<0xE0, 0> {
 public:
    void invoke(gsl::span<const std::uint8_t>) {
        LOG_INFO("Performing full power cycle on demand.");

        FullPowerCycle::perform();
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_POWERCYCLE_H_
