#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

class RadFET_On : public Command<0x84, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        pld::Telemetry::Radfet data;
        LOG_INFO("Command RadFET_On");
        hardware.radfet_on();
        data.status      = pld::Telemetry::RadfetState::TURNED_ON;
        telemetry.radfet = data;
    }
};

class RadFET_Measure : public Command<0x85, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("Command RadFET_Measure START");
        telemetry.radfet = hardware.radfet_read();
        LOG_INFO("Command RadFET_Measure FINISHED");
    }
};

class RadFET_Off : public Command<0x86, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        pld::Telemetry::Radfet data;
        LOG_INFO("Command RadFET_Off");
        hardware.radfet_off();
        data.status      = pld::Telemetry::RadfetState::TURNED_OFF;
        telemetry.radfet = data;
    }
};
}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
