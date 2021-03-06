#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_HOUSEKEEPING_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_HOUSEKEEPING_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

class HouseKeeping : public Command<0x83, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("HouseKeeping");

        Telemetry::Housekeeping hk;
        hardware.read_adc(
            {{hardware::AnalogChannel::HouseKeeping_3V3d, &hk.int_3v3d},
             {hardware::AnalogChannel::HouseKeeping_3V3_OBC, &hk.obc_3v3d}});

        telemetry.housekeeping = hk;
    }
};

}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_HOUSEKEEPING_H_
