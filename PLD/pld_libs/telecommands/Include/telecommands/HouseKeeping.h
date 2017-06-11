#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_HOUSEKEEPING_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_HOUSEKEEPING_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class HouseKeeping : public PldCommand<0x83> {
 public:
    HouseKeeping(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }

    void work(gsl::span<const std::uint8_t>) override {
        LOG_INFO("HouseKeeping");

        Telemetry::Housekeeping hk;
        hardware->read_adc(
            {{hardware::AnalogChannel::HouseKeeping_3V3d, &hk.int_3v3d},
             {hardware::AnalogChannel::HouseKeeping_3V3_OBC, &hk.obc_3v3d}});

        telemetry.housekeeping = hk;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_HOUSEKEEPING_H_
