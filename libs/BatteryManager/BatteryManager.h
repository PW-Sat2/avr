#ifndef LIBS_BATTERYMANAGER_BATTERYMANAGER_H_
#define LIBS_BATTERYMANAGER_BATTERYMANAGER_H_

#include "TwoStateFsm.h"
#include "logger.h"

namespace avr {

/*!
 * Battery manager.
 * Implements battery charging algorithm and battery heater.
 * @tparam PinCharge Pin used to enable battery charging LCL.
 * @tparam PinHeater Pin used to enable battery heater.
 */
template<typename PinCharge, typename PinHeater>
class BatteryManager {
 public:
    BatteryManager()
        : battery_charger(state_standby, state_charging),
          battery_heater(state_heating_off, state_heating_on) {
    }

    /*!
     * Tick. Should be invoked every 10 seconds.
     * @param battery_voltage Actual battery voltage.
     * @param max_battery_temperature Maximal battery temperature (possibly
     * calculated from different sensors).
     */
    void tick(float battery_voltage, float max_battery_temperature) {
        battery_charger.tick(battery_voltage, max_battery_temperature);
        battery_heater.tick(max_battery_temperature);
    }

 private:
    using BatteryChargeFSM = TwoStateFsm<float, float>;

    struct StateChargingON : BatteryChargeFSM::IState {
        void action() override {
            LOG_INFO("[BATC] Charging ON");
            PinCharge::set();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage > 8.0 or max_temperature > 43);
        }
    };

    struct StateChargingOFF : BatteryChargeFSM::IState {
        void action() override {
            LOG_INFO("[BATC] Charging OFF");
            PinCharge::reset();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage < 7.8 and max_temperature < 40);
        }
    };

    StateChargingON state_charging;
    StateChargingOFF state_standby;
    BatteryChargeFSM battery_charger;


    using BatteryHeaterFSM = TwoStateFsm<float>;

    struct StateHeaterON : BatteryHeaterFSM::IState {
        void action() override {
            LOG_INFO("[BATC] Heating ON");
            PinHeater::set();
        }

        bool should_change(float max_temperature) override {
            return (max_temperature > 5);
        }
    };

    struct StateHeaterOFF : BatteryHeaterFSM::IState {
        void action() override {
            LOG_INFO("[BATC] Heating OFF");
            PinHeater::reset();
        }

        bool should_change(float max_temperature) override {
            return (max_temperature < 0);
        }
    };

    StateHeaterON state_heating_on;
    StateHeaterOFF state_heating_off;
    BatteryHeaterFSM battery_heater;
};

}  // namespace avr

#endif  // LIBS_BATTERYMANAGER_BATTERYMANAGER_H_
