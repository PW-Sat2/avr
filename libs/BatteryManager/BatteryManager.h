#ifndef LIBS_BATTERYMANAGER_BATTERYMANAGER_H_
#define LIBS_BATTERYMANAGER_BATTERYMANAGER_H_

#include <hal/hal>
#include "TwoStateFsm.h"
#include "logger.h"

namespace eps {

/*!
 * Battery manager.
 * Implements battery charging algorithm and battery heater.
 * @tparam PinCharge Pin used to enable battery charging LCL.
 * @tparam BatteryHeater BatteryHeater object.
 * @tparam PinDischarge Pin used to enable discharging battery.
 */
template<typename PinCharge, typename PinDischarge, typename BatteryHeater>
class BatteryManager {
 public:
    BatteryManager()
        : battery_charger(state_standby, state_charging),
          battery_heater(state_heating_off, state_heating_on),
          battery_discharger(state_discharging_off, state_discharging_on) {
    }

    /*!
     * Tick. Should be invoked every 10 seconds.
     * @param battery_voltage Actual battery voltage.
     * @param max_battery_temperature Maximal battery temperature (possibly
     * calculated from different sensors).
     */
    void tick(float battery_voltage, float max_battery_temperature) {
        battery_charger.tick(battery_voltage, max_battery_temperature);
        battery_discharger.tick(battery_voltage, max_battery_temperature);
        battery_heater.tick(max_battery_temperature);
        LOG_DEBUG("[BATC] Temp %.2f; Vbat %.2f; CHG %d; DCHG %d; HTR %d",
                  max_battery_temperature,
                  battery_voltage,
                  hal::libs::read_bit<0>(state()),
                  hal::libs::read_bit<1>(state()),
                  hal::libs::read_bit<2>(state()));
    }

    uint8_t state() {
        auto charging    = static_cast<uint8_t>(PinCharge::read());
        auto discharging = static_cast<uint8_t>(PinDischarge::read());
        auto heater      = static_cast<uint8_t>(BatteryHeater::heating());

        return (heater << 2) | (discharging << 1) | charging;
    }

 private:
    using BatteryChargeFSM = avr::TwoStateFsm<float, float>;

    struct StateChargingON : BatteryChargeFSM::IState {
        void action() override {
            PinCharge::set();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage > 8.0 or max_temperature > 43);
        }
    };

    struct StateChargingOFF : BatteryChargeFSM::IState {
        void action() override {
            PinCharge::reset();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage < 7.8 and max_temperature < 40);
        }
    };

    StateChargingON state_charging;
    StateChargingOFF state_standby;
    BatteryChargeFSM battery_charger;


    using BatteryHeaterFSM = avr::TwoStateFsm<float>;

    struct StateHeaterON : BatteryHeaterFSM::IState {
        void action() override {
            BatteryHeater::enable();
        }

        bool should_change(float max_temperature) override {
            return (max_temperature > -2);
        }
    };

    struct StateHeaterOFF : BatteryHeaterFSM::IState {
        void action() override {
            BatteryHeater::disable();
        }

        bool should_change(float max_temperature) override {
            return (max_temperature < -7);
        }
    };

    StateHeaterON state_heating_on;
    StateHeaterOFF state_heating_off;
    BatteryHeaterFSM battery_heater;


    using BatteryDischargeFSM = avr::TwoStateFsm<float, float>;

    struct StateDischargingON : BatteryDischargeFSM::IState {
        void action() override {
            PinDischarge::set();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage < 6.3 or max_temperature > 58);
        }
    };

    struct StateDischargingOFF : BatteryDischargeFSM::IState {
        void action() override {
            PinDischarge::reset();
        }

        bool should_change(float battery_voltage, float max_temperature) override {
            return (battery_voltage > 6.8 and max_temperature < 55);
        }
    };

    StateDischargingON state_discharging_on;
    StateDischargingOFF state_discharging_off;
    BatteryDischargeFSM battery_discharger;
};

}  // namespace eps

#endif  // LIBS_BATTERYMANAGER_BATTERYMANAGER_H_
