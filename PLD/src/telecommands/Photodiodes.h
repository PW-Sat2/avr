#ifndef PLD_SRC_TELECOMMANDS_PHOTODIODES_H_
#define PLD_SRC_TELECOMMANDS_PHOTODIODES_H_

#include <ObcInterface/CommandDispatcher.h>
#include <cstdio>

#include "../hw.h"
#include "../telemetry.h"

namespace PLD {
namespace telecommands {

class Photodiodes : public CommandWithOpcode<0x82, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        std::printf("Photodiodes\n");

        hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::Photodiode_B);
        telemetryBuffer.buffered->photodiodes.Xp = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::Photodiode_A);
        telemetryBuffer.buffered->photodiodes.Xn = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::Photodiode_C);
        telemetryBuffer.buffered->photodiodes.Yp = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::Photodiode_D);
        telemetryBuffer.buffered->photodiodes.Yn = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::Photodiode_D);

        telemetryBuffer.update();
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_PHOTODIODES_H_
