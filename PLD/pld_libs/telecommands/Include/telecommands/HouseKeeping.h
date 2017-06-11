#ifndef PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_
#define PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class HouseKeeping : public PldCommand<0x83> {
 public:
    HouseKeeping(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }

    void work(gsl::span<const std::uint8_t>) override {
        std::printf("HouseKeeping\n");

        Telemetry::Housekeeping hk;
        hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::HouseKeeping_3V3d);

        hk.int_3v3d = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::HouseKeeping_3V3_OBC);

        hk.obc_3v3d = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::HouseKeeping_3V3_OBC);

        telemetry.housekeeping = hk;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_
