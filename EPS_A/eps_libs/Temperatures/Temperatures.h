#ifndef EPS_A_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_
#define EPS_A_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_

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
    battery_pack_a(tmp121,
                   lower_battery_pack,
                   higher_battery_pack,
                   battery_pack_a_name,
                   Eps::telemetry.general.get().battery_pack.temperature_a);

constexpr static char battery_pack_b_name[] PROGMEM = "BP B";
constexpr static TemperatureConversion
    battery_pack_b(tmp121,
                   lower_battery_pack,
                   higher_battery_pack,
                   battery_pack_b_name,
                   Eps::telemetry.general.get().battery_pack.temperature_b);

constexpr static char dcdc3v3_name[] PROGMEM = "DCDC 3v3";
constexpr static TemperatureConversion
    dcdc3v3(lmt87,
            lower_eps,
            higher_eps,
            dcdc3v3_name,
            Eps::telemetry.general.get().dcdc_3v3_temperature);

constexpr static char dcdc5v_name[] PROGMEM = "DCDC 5v";
constexpr static TemperatureConversion
    dcdc5v(lmt87,
           lower_eps,
           higher_eps,
           dcdc5v_name,
           Eps::telemetry.general.get().dcdc_5v_temperature);

constexpr static char batc_name[] PROGMEM = "BATC";
constexpr static TemperatureConversion
    batc(lmt87,
         lower_eps,
         higher_eps,
         batc_name,
         Eps::telemetry.general.get().battery_controller.temperature);

constexpr static char ctrla_name[] PROGMEM = "CTRL A";
constexpr static TemperatureConversion
    ctrla(lmt87,
          lower_eps,
          higher_eps,
          ctrla_name,
          Eps::telemetry.general.get().controller_a.temperature);

constexpr static char supply_name[] PROGMEM = "SUPPLY";
constexpr static TemperatureConversion
    supply(lmt87,
           lower_eps,
           higher_eps,
           supply_name,
           Eps::telemetry.general.get().controller_a.supply_temperature);

constexpr static char mppt_x_name[] PROGMEM = "MPPT X";
constexpr static TemperatureConversion
    mppt_x(lmt87_adc124,
           lower_eps,
           higher_eps,
           mppt_x_name,
           Eps::telemetry.mppt.get().mpptx.temperature);

constexpr static char mppt_yp_name[] PROGMEM = "MPPT Y+";
constexpr static TemperatureConversion
    mppt_yp(lmt87_adc124,
            lower_eps,
            higher_eps,
            mppt_yp_name,
            Eps::telemetry.mppt.get().mpptyp.temperature);

constexpr static char mppt_yn_name[] PROGMEM = "MPPT Y-";
constexpr static TemperatureConversion
    mppt_yn(lmt87_adc124,
            lower_eps,
            higher_eps,
            mppt_yn_name,
            Eps::telemetry.mppt.get().mpptyn.temperature);

}  // namespace temperatures
}  // namespace avr

#endif  // EPS_A_EPS_LIBS_TEMPERATURES_TEMPERATURES_H_
