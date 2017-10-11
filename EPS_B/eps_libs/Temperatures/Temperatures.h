#ifndef EPS_B_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_
#define EPS_B_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_

#include "TemperatureSensorsValidation.h"

#include "eps.h"

namespace avr {
namespace temperatures {

constexpr static float lower_eps  = -50;
constexpr static float higher_eps = 100;

constexpr static float lower_battery_pack  = -50;
constexpr static float higher_battery_pack = 80;

constexpr static char battery_pack_a_name[] PROGMEM = "BP A";
constexpr static TemperatureConversion
    battery_pack(pt1000,
                 lower_battery_pack,
                 higher_battery_pack,
                 battery_pack_a_name,
                 Eps::telemetry.general.get().battery_pack.temperature);


constexpr static char ctrla_name[] PROGMEM = "CTRL A";
constexpr static TemperatureConversion
    ctrlb(lmt87,
          lower_eps,
          higher_eps,
          ctrla_name,
          Eps::telemetry.general.get().controller_b.temperature);

constexpr static char supply_name[] PROGMEM = "SUPPLY";
constexpr static TemperatureConversion
    supply(lmt87,
           lower_eps,
           higher_eps,
           supply_name,
           Eps::telemetry.general.get().controller_b.supply_temperature);

}  // namespace temperatures
}  // namespace avr

#endif  // EPS_B_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_
