#ifndef PLD_SRC_TELECOMMANDS_RADFET_H_
#define PLD_SRC_TELECOMMANDS_RADFET_H_

#include <cstdio>
#include "../hw.h"
#include "../telemetry.h"
#include "ObcInterface/CommandDispatcher.h"

namespace PLD {
namespace telecommands {

class RadFET : public CommandWithOpcode<0x83, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        std::printf("RadFET\n");
        Telemetry::Radfet radfet;
        radfet.temperature = 0x55AA;

        telemetryBuffer.buffered->radfet = radfet;
        telemetryBuffer.update();
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_RADFET_H_
