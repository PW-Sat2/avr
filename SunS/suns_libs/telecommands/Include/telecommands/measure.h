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

        suns::Telemetry::LightData vl, ir;
        suns::Telemetry::Temperatures temperature;
        uint32_t status = 0;
        LOG_INFO("SunS Measure\n");
        hardware.temperatures_measure(temperature);
        LOG_INFO("Temperature Measured\n");
        hardware.als_measure(args[0], args[1], status, vl, ir);

        telemetry.status = status;
        telemetry.vl_data = vl;
        telemetry.ir_data = ir;
        telemetry.temperature_data = temperature;
        telemetry.parameters = {args[0], args[1]};

        LOG_INFO("ALS Measured\n");
    }
};

}  // namespace telecommands
}  // namespace suns

#endif  // SUNS_SUNS_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_SUNSREF_H_
