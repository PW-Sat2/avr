#ifndef LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
#define LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

constexpr static uint8_t arm_code[]     = {0xFE, 0xE1, 0xDE, 0xAD};
constexpr static uint8_t disable_code[] = {0xBA, 0xAD, 0xC0, 0x01, 0xC0, 0xDE};

template<bool& flag_armed>
struct ArmDisablePowerCycle : public Command<0xEA, sizeof(arm_code)> {
    void invoke(gsl::span<const std::uint8_t> cmd) {
        LOG_FATAL("ARM power cycle disabling command received");
        if (cmd == gsl::make_span(arm_code)) {
            flag_armed = true;
            LOG_FATAL("Power Cycle Disable ARMED");
        } else {
            flag_armed = false;
            LOG_FATAL("Power Cycle Disable DISARMED");
        }
    }
};

template<const bool& flag_armed, bool& flag_disabled>
struct DisablePowerCycle : public Command<0xEB, sizeof(disable_code)> {
    void invoke(gsl::span<const std::uint8_t> cmd) {
        LOG_FATAL("Disable power cycle command received");
        if (hal::libs::ToInt(flag_armed) == 1 &&
            cmd == gsl::make_span(disable_code)) {
            flag_disabled = true;
            LOG_FATAL("Power Cycle Disabled!");
        } else {
            flag_disabled = false;
            LOG_FATAL("Power Cycle Enabled!");
        }
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_DISABLEPOWERCYCLE_H_
