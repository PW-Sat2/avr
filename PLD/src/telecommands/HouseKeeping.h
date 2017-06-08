#ifndef PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_
#define PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_

#include <ObcInterface/CommandDispatcher.h>
#include <cstdio>

#include "hw.h"
#include "telemetry.h"

namespace PLD {
namespace telecommands {

class HouseKeeping : public CommandWithOpcode<0x84, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        std::printf("HouseKeeping\n");

        hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::HouseKeeping_3V3d);
        telemetryBuffer.buffered->voltages.int_3v3d =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::HouseKeeping_3V3_OBC);
        telemetryBuffer.buffered->voltages.obc_3v3d =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::HouseKeeping_3V3_OBC);

        telemetryBuffer.update();
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_HOUSEKEEPING_H_
