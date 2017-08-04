#ifndef EPS_A_EPS_LIBS_COMMANDS_COMMANDS_LCL_H_
#define EPS_A_EPS_LIBS_COMMANDS_COMMANDS_LCL_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps_a {
namespace commands {

template<typename LclCommander>
class EnableLCL : public Command<0xE1, 1> {
 public:
    void invoke(gsl::span<const std::uint8_t> args) {
        LclCommander::on(args[0]);
    }
};

template<typename LclCommander>
class DisableLCL : public Command<0xE2, 1> {
 public:
    void invoke(gsl::span<const std::uint8_t> args) {
        LclCommander::off(args[0]);
    }
};

}  // namespace commands
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_COMMANDS_COMMANDS_LCL_H_
