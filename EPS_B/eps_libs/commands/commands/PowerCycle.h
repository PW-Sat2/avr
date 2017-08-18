#ifndef EPS_B_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_
#define EPS_B_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps_b {
namespace commands {

class PowerCycle : public Command<0xE0, 0> {
 public:
    void invoke(gsl::span<const std::uint8_t>) {
        LOG_INFO("Performing full power cycle on demand.");
    }
};

}  // namespace commands
}  // namespace eps_b

#endif  // EPS_B_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_
