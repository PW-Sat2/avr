#ifndef SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_
#define SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace suns {
namespace commands {

class Measure : public Command<0x80, 2> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t> args) {
        LOG_INFO("[start] SunS measure");

        suns::Telemetry::LightData vl, ir;
        suns::Telemetry::Temperatures temperature;
        suns::Telemetry::Status als_status = {0, 0, 0};

        LOG_INFO("[start] measure temperature");
        hardware.temperatures_measure(temperature);
        LOG_INFO("[finished] measure temperature");

        LOG_INFO("[start] measure als");
        hardware.als_measure(args[0], args[1], als_status, vl, ir);
        LOG_INFO("[finished] measure als");

        telemetry.als_status       = als_status;
        telemetry.vl_data          = vl;
        telemetry.ir_data          = ir;
        telemetry.temperature_data = temperature;
        telemetry.parameters       = {args[0], args[1]};

        LOG_INFO("[finished] SunS measure");
    }
};

}  // namespace commands
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_COMMANDS_INCLUDE_COMMANDS_MEASURE_H_
