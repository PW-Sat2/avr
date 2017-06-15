#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace telecommands {

class RadFET : public Command<0x84, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET");
        telemetry.radfet = hardware.read_radfet();
    }
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_
