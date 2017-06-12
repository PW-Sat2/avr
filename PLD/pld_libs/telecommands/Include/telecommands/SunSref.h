#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_

#include <ObcInterface/Command.h>
#include <logger.h>

namespace pld {
namespace telecommands {

class SunSRef : public Command<0x80, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("SunS");

        Telemetry::SunsRef suns;

        using hardware::AnalogChannel;

        hardware.read_adc({{AnalogChannel::SunSRef_V0, &suns.voltages[0]},
                           {AnalogChannel::SunSRef_V1, &suns.voltages[1]},
                           {AnalogChannel::SunSRef_V2, &suns.voltages[2]},
                           {AnalogChannel::SunSRef_V3, &suns.voltages[3]},
                           {AnalogChannel::SunSRef_V4, &suns.voltages[4]}});

        telemetry.suns_ref = suns;
    }
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
