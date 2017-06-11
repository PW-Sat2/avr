#ifndef PLD_SRC_TELECOMMANDS_RADFET_H_
#define PLD_SRC_TELECOMMANDS_RADFET_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class RadFET : public PldCommand<0x84> {
 public:
    RadFET(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }

    void work(gsl::span<const std::uint8_t>) override {
        std::printf("RadFET\n");
        telemetry.radfet = hardware->read_radfet();
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_RADFET_H_
