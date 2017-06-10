#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_

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

        using hardware::AnalogChannel;

        hardware->read_adc({{AnalogChannel::Photodiode_B, &photodiodes.Xp},
                            {AnalogChannel::Photodiode_A, &photodiodes.Xn},
                            {AnalogChannel::Photodiode_C, &photodiodes.Yp},
                            {AnalogChannel::Photodiode_D, &photodiodes.Yn}});

        telemetry.photodiodes = photodiodes;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_
