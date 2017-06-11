#ifndef PLD_SRC_TELECOMMANDS_PHOTODIODES_H_
#define PLD_SRC_TELECOMMANDS_PHOTODIODES_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class Photodiodes : public PldCommand<0x82> {
 public:
    Photodiodes(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }

    void work(gsl::span<const std::uint8_t>) override {
        std::printf("Photodiodes\n");

        Telemetry::Photodiodes photodiodes;
        hardware->read_adc_and_change_channel(hardware::AnalogChannel::Photodiode_B);
        photodiodes.Xp = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::Photodiode_A);
        photodiodes.Xn = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::Photodiode_C);
        photodiodes.Yp = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::Photodiode_D);
        photodiodes.Yn = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::Photodiode_D);

        telemetry.photodiodes = photodiodes;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_PHOTODIODES_H_
