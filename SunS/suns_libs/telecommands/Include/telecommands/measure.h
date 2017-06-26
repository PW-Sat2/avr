#ifndef SUNS_SUNS_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
#define SUNS_SUNS_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace suns {
namespace telecommands {

class Measure : public Command<0x80, 2> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t> args) {
        LOG_INFO("SunS Measure");

        //telemetry.temperatures = hardware.read_temperatures();
        LOG_INFO("Temperature Measured");
        telemetry.als = hardware.als_measure(args[0], args[1]);
        LOG_INFO("ALS Measured");
        printf("%d %d\n", args[0], args[1]);
    }
};

}  // namespace telecommands
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
