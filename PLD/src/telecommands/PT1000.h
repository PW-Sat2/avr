#ifndef PLD_SRC_TELECOMMANDS_PT1000_H_
#define PLD_SRC_TELECOMMANDS_PT1000_H_

#include <ObcInterface/CommandDispatcher.h>
#include <cstdio>

#include "../hw.h"
#include "../telemetry.h"

namespace PLD {
namespace telecommands {

class PT1000 : public CommandWithOpcode<0x81, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        std::printf("PT1000\n");

        hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::TemperatureSADS);
        telemetryBuffer.buffered->temperatures.sads =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::TemperatureSail);
        telemetryBuffer.buffered->temperatures.sail =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::TemperatureXn);
        telemetryBuffer.buffered->temperatures.Xn = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::TemperatureXp);
        telemetryBuffer.buffered->temperatures.Xp = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::TemperatureYn);
        telemetryBuffer.buffered->temperatures.Yn = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::TemperatureYp);
        telemetryBuffer.buffered->temperatures.Yp = hw.read_adc_and_change_channel(
            PLDHardware::AnalogChannel::TemperatureCamNadir);
        telemetryBuffer.buffered->temperatures.cam_nadir =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::TemperatureCamWing);
        telemetryBuffer.buffered->temperatures.cam_wing =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::TemperatureSupply);

        telemetryBuffer.buffered->temperatures.supply =
            hw.read_adc_and_change_channel(
                PLDHardware::AnalogChannel::TemperatureSupply);

        telemetryBuffer.update();
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_PT1000_H_
