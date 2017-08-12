#ifndef EPS_A_EPS_LIBS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_
#define EPS_A_EPS_LIBS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps_a {
namespace commands {

template<typename OverheatProtection>
struct DisableOverheatProtection : public Command<0xE4, 0> {
    void invoke(gsl::span<const std::uint8_t>) {
        OverheatProtection::disable();
    }
};

}  // namespace commands
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_
