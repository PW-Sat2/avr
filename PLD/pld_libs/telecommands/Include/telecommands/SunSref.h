#ifndef PLD_SRC_TELECOMMANDS_SUNSREF_H_
#define PLD_SRC_TELECOMMANDS_SUNSREF_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class SunSRef : public PldCommand<0x80> {
 public:
    SunSRef(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }

    void work(gsl::span<const std::uint8_t>) override {
        std::printf("SunS\n");

        Telemetry::SunsRef suns;

        using hardware::AnalogChannel;

        hardware->read_adc({{AnalogChannel::SunSRef_V0, &suns.voltages[0]},
                            {AnalogChannel::SunSRef_V1, &suns.voltages[1]},
                            {AnalogChannel::SunSRef_V2, &suns.voltages[2]},
                            {AnalogChannel::SunSRef_V3, &suns.voltages[3]},
                            {AnalogChannel::SunSRef_V4, &suns.voltages[4]}});

        telemetry.suns_ref = suns;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_SUNSREF_H_
