#ifndef LIBS_EPS_COMMANDS_COMMANDS_THERMALKNIVES_H_
#define LIBS_EPS_COMMANDS_COMMANDS_THERMALKNIVES_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace eps {
namespace commands {

template<typename TKModule>
struct ThermalKnives : public Command<0xE3, 1> {
    void invoke(gsl::span<const std::uint8_t> data) {
        TKModule::burn(data[0]);
    }
};

}  // namespace commands
}  // namespace eps

#endif  // LIBS_EPS_COMMANDS_COMMANDS_THERMALKNIVES_H_
