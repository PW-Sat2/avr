#ifndef LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
#define LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

constexpr static uint8_t arm_code[]     = {0xDE, 0xAD, 0xBE, 0xEF};
constexpr static uint8_t disable_code[] = {0xDE, 0xFA, 0xCE, 0xD, 0xF0, 0x0D};

template<bool& flag_armed>
struct ArmDisablePowerCycle : public Command<0xEA, 5> {
    void invoke(gsl::span<const std::uint8_t> cmd) {
        if (cmd == gsl::make_span(arm_code)) {
            flag_armed = true;
        } else {
            flag_armed = false;
        }
    }
};

template<const bool& flag_armed, bool& flag_enabled>
struct DisablePowerCycle : public Command<0xE8, 5> {
    void invoke(gsl::span<const std::uint8_t> cmd) {
        if (flag_armed && cmd == gsl::make_span(disable_code)) {
            flag_enabled = false;
        } else {
            flag_enabled = true;
        }
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
