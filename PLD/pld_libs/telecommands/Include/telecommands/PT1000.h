#ifndef PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PT1000_H_
#define PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PT1000_H_

#include <ObcInterface/Command.h>
#include <logger.h>

namespace pld {
namespace telecommands {

class PT1000 : public Command<0x81, 0> {
 public:
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("Temperatures");

        using hardware::AnalogChannel;

        Telemetry::Temperatures temperatures;
        hardware.read_adc(
            {{AnalogChannel::TemperatureSADS, &temperatures.sads},
             {AnalogChannel::TemperatureSail, &temperatures.sail},
             {AnalogChannel::TemperatureXp, &temperatures.Xp},
             {AnalogChannel::TemperatureXn, &temperatures.Xn},
             {AnalogChannel::TemperatureYp, &temperatures.Yp},
             {AnalogChannel::TemperatureYn, &temperatures.Yn},
             {AnalogChannel::TemperatureCamWing, &temperatures.cam_wing},
             {AnalogChannel::TemperatureCamNadir, &temperatures.cam_nadir},
             {AnalogChannel::TemperatureSupply, &temperatures.supply}});

        telemetry.temperatures = temperatures;
    }
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_PLD_LIBS_TELECOMMANDS_INCLUDE_TELECOMMANDS_PT1000_H_
