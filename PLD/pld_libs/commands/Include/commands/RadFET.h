#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

struct RadFET_On : public Command<0x84, 0> {
    void invoke(Telemetry&,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_On");
        hardware.radfet_on();
    }
};

struct RadFET_Measure : public Command<0x85, 0> {
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_Measure START");
        telemetry.radfet = hardware.radfet_read();
        LOG_INFO("RadFET_Measure FINISHED");
    }
};

struct RadFET_Off : public Command<0x86, 0> {
    void invoke(Telemetry&,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_Off");
        hardware.radfet_off();
    }
};

}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
