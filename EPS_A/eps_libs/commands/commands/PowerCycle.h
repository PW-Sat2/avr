#ifndef EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_
#define EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "PowerCycle/PowerCycle.h"
#include "logger.h"

namespace eps_a {
namespace commands {

class PowerCycle : public Command<0xE0, 0> {
 public:
    void invoke(gsl::span<const std::uint8_t>) {
        LOG_INFO("Performing full power cycle on demand.");

        full_power_cycle();
    }
};

}  // namespace commands
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_
