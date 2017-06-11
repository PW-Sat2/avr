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

        Telemetry::Temperatures temperatures;
        hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureSADS);
        temperatures.sads = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureSail);
        temperatures.sail = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureXn);
        temperatures.Xn = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureXp);
        temperatures.Xp = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureYn);
        temperatures.Yn = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureYp);
        temperatures.Yp = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureCamNadir);
        temperatures.cam_nadir = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureCamWing);
        temperatures.cam_wing = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureSupply);
        temperatures.supply = hardware->read_adc_and_change_channel(
            hardware::AnalogChannel::TemperatureSupply);

        telemetry.temperatures = temperatures;
    }

 private:
    Telemetry& telemetry;
    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_SRC_TELECOMMANDS_PT1000_H_
