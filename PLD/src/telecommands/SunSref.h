#ifndef PLD_SRC_TELECOMMANDS_SUNSREF_H_
#define PLD_SRC_TELECOMMANDS_SUNSREF_H_

#include <ObcInterface/CommandDispatcher.h>
#include <cstdio>

#include "../hw.h"
#include "../telemetry.h"

namespace PLD {
namespace telecommands {

class SunSRef : public CommandWithOpcode<0x80, 0> {
 public:
    void invoke(gsl::span<std::uint8_t>) override {
        std::printf("SunS\n");

        PLD::Telemetry::SunsRef suns;

        hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V0);
        suns.voltages[0] =
            hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V1);
        suns.voltages[1] =
            hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V2);
        suns.voltages[2] =
            hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V3);
        suns.voltages[3] =
            hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V4);
        suns.voltages[4] =
            hw.read_adc_and_change_channel(PLDHardware::AnalogChannel::SunSRef_V4);

        telemetryBuffer.buffered->suns_ref = suns;
        telemetryBuffer.update();
    }
};

}  // namespace telecommands
}  // namespace PLD

#endif  // PLD_SRC_TELECOMMANDS_SUNSREF_H_
