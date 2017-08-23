#ifndef LIBS_EPS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_
#define LIBS_EPS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

template<typename OverheatProtection>
struct DisableOverheatProtection : public Command<0xE4, 0> {
    void invoke(gsl::span<const std::uint8_t>) {
        OverheatProtection::disable();
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_DISABLEOVERHEATPROTECTION_H_
