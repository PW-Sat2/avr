#ifndef PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
#define PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_

#include "ObcInterface/Command.h"
#include "logger.h"

namespace pld {
namespace commands {

struct RadFET_On : public Command<0x84, 0> {
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_On");
        hardware.radfet_on();

        telemetry.radfet.status.set_status(
            Telemetry::Radfet::Status::Fields::On, true);
    }
};

struct RadFET_Measure : public Command<0x85, 0> {
    using Fields = Telemetry::Radfet::Status::Fields;

    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_Measure START");


        auto tm = hardware.radfet_read();
        telemetry.radfet.status.set_status(Fields::TimeoutVth0, tm.timeout_vth0);
        telemetry.radfet.status.set_status(Fields::TimeoutVth1, tm.timeout_vth1);
        telemetry.radfet.status.set_status(Fields::TimeoutVth2, tm.timeout_vth2);
        telemetry.radfet.status.set_status(Fields::TimeoutTemperature,
                                           tm.timeout_temperature);

        telemetry.radfet.status.set_status(
            Telemetry::Radfet::Status::Fields::LCL_3V3_error, tm.overcurrent_3v3);
        telemetry.radfet.status.set_status(
            Telemetry::Radfet::Status::Fields::LCL_5V_error, tm.overcurrent_5v);

        telemetry.radfet.status.set_status(Fields::MeasurementDone, true);

        telemetry.radfet.measurement = tm.measurement;

        LOG_INFO("RadFET_Measure FINISHED");
    }
};

struct RadFET_Off : public Command<0x86, 0> {
    void invoke(Telemetry& telemetry,
                hardware::Interface& hardware,
                gsl::span<const std::uint8_t>) {
        LOG_INFO("RadFET_Off");
        hardware.radfet_off();

        telemetry.radfet.status.set_status(
            Telemetry::Radfet::Status::Fields::On, false);
    }
};

}  // namespace commands
}  // namespace pld

#endif  // PLD_PLD_LIBS_COMMANDS_INCLUDE_COMMANDS_RADFET_H_
