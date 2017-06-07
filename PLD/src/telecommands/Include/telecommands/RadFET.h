#ifndef PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_
#define PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_

#include "ObcInterface/CommandDispatcher.h"
#include "telemetry/telemetry.h"

namespace PLD {
namespace telecommands {

class RadFET : public CommandWithOpcode<0x81, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        printf("RadFET\n");
        Telemetry::Radfet radfet;
        radfet.temperature = 0x55AA;

        PLD::telemetry.radfet = radfet;
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_INCLUDE_TELECOMMANDS_RADFET_H_
