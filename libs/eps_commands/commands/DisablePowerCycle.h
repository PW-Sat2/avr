#ifndef LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
#define LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

constexpr static uint8_t arm_code[]     = {0xFE, 0xE1, 0xDE, 0xAD};
constexpr static uint8_t disable_code[] = {0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE};

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

template<const bool& flag_armed, bool& flag_disabled>
struct DisablePowerCycle : public Command<0xEB, 5> {
    void invoke(gsl::span<const std::uint8_t> cmd) {
        if (hal::libs::ToInt(flag_armed) == 1 &&
            cmd == gsl::make_span(disable_code)) {
            flag_disabled = true;
        } else {
            flag_disabled = false;
        }
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
