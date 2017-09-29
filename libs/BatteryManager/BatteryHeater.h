#ifndef LIBS_BATTERYMANAGER_BATTERYHEATER_H_
#define LIBS_BATTERYMANAGER_BATTERYHEATER_H_

namespace eps {
namespace battery_heater {

enum class LogicLevel {
    ActiveHigh,
    ActiveLow,
};

template<typename PinHeater, LogicLevel active_high>
struct BatteryHeater;

template<typename PinHeater>
struct BatteryHeater<PinHeater, LogicLevel::ActiveHigh> {
    static void init() {
        disable();
        PinHeater::init(hal::DigitalIO::Mode::OUTPUT);
    }

    static void enable() {
        PinHeater::set();
    }

    static void disable() {
        PinHeater::reset();
    }

    static bool heating() {
        return (PinHeater::read() == true);
    }
};

template<typename PinHeater>
struct BatteryHeater<PinHeater, LogicLevel::ActiveLow> {
    static void init() {
        disable();
        PinHeater::init(hal::DigitalIO::Mode::OUTPUT);
    }

    static void enable() {
        PinHeater::reset();
    }

    static void disable() {
        PinHeater::set();
    }

    static bool heating() {
        return (PinHeater::read() == false);
    }
};

}  // namespace battery_heater
}  // namespace eps

#endif  // LIBS_BATTERYMANAGER_BATTERYHEATER_H_
