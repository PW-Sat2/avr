#ifndef SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_
#define SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace suns {
namespace commands {

class Measure : public Command<0x80, 2> {
 public:
    void invoke(Telemetry&,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("SunS measure start");
        hardware.measure();
    }
};

}  // namespace commands
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_
