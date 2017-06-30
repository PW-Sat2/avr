#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_PHOTODIODES_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_PHOTODIODES_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

class Photodiodes : public Command<0x82, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("Photodiodes");

        Telemetry::Photodiodes photodiodes;

        using hardware::AnalogChannel;

        hardware.read_adc({{AnalogChannel::PhotodiodeXp, &photodiodes.Xp},
                           {AnalogChannel::PhotodiodeXn, &photodiodes.Xn},
                           {AnalogChannel::PhotodiodeYp, &photodiodes.Yp},
                           {AnalogChannel::PhotodiodeYn, &photodiodes.Yn}});

        telemetry.photodiodes = photodiodes;
    }
};

}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_PHOTODIODES_H_
