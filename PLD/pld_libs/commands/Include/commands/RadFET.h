#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

class RadFET : public Command<0x84, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET");
        telemetry.radfet = hardware.read_radfet();
    }
};

}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
