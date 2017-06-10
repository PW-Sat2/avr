#ifndef PLD_SRC_TELECOMMANDS_PT1000_H_
#define PLD_SRC_TELECOMMANDS_PT1000_H_

#include "pld_command.h"

namespace pld {
namespace telecommands {

class PT1000 : public PldCommand<0x81> {
 public:
    PT1000(Telemetry& telemetry, hardware::Interface*& hardware)
        : PldCommand(hardware), telemetry(telemetry), hardware(hardware) {
    }
    void work(gsl::span<const std::uint8_t>) override {
        std::printf("PT1000\n");

        using hardware::AnalogChannel;

        Telemetry::Temperatures temperatures;
        hardware->read_adc({{AnalogChannel::TemperatureSADS, &temperatures.sads},
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

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_PT1000_H_
