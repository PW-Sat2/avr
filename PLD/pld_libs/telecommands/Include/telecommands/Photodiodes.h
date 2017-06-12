#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_

#include <ObcInterface/Command.h>
#include <logger.h>

namespace pld {
namespace telecommands {

class Photodiodes : public Command<0x82, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("Photodiodes");

        Telemetry::Photodiodes photodiodes;

        using hardware::AnalogChannel;

        hardware.read_adc({{AnalogChannel::Photodiode_B, &photodiodes.Xp},
                           {AnalogChannel::Photodiode_A, &photodiodes.Xn},
                           {AnalogChannel::Photodiode_C, &photodiodes.Yp},
                           {AnalogChannel::Photodiode_D, &photodiodes.Yn}});

        telemetry.photodiodes = photodiodes;
    }
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PHOTODIODES_H_
