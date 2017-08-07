#ifndef EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWxERCYCLE_H_
#define EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWxERCYCLE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps_a {
namespace commands {

template<typename TKModule>
class ThermalKnives : public Command<0xE3, 1> {
 public:
    void invoke(gsl::span<const std::uint8_t> data) {
        switch (data[0]) {
            case 1:
                TKModule::burn(TKModule::Types::Sail);
                break;
            case 2:
                TKModule::burn(TKModule::Types::Sads);
                break;
            default:
                LOG_ERROR("Unknown BURN id %d\n", data[0]);
        }
    }
};

}  // namespace commands
}  // namespace eps_a

#endif  // EPS_A_EPS_LIBS_COMMANDS_COMMANDS_POWERCYCLE_H_
