#ifndef LIBS_EPS_EPS_H_
#define LIBS_EPS_EPS_H_

#include <BatteryManager.h>
#include "MainTimer.h"
#include "ObcInterface/CommandDispatcher.h"
#include "ObcInterface/ObcInterface.h"

#include "commands/DisableOverheatProtection.h"
#include "commands/LCL.h"
#include "commands/ObcWatchdog.h"
#include "commands/PowerCycle.h"
#include "commands/ThermalKnives.h"

#include "LclCommander.h"
#include "ObcWatchdog.h"
#include "OverheatProtection.h"
#include "PowerCycle.h"
#include "TelemetryUpdater.h"
#include "ThermalKnives.h"

#include "Prescaler.h"

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
struct Epss {
    using This = Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>;

    static void CommandCallback(gsl::span<const uint8_t> _c) {
        dispatcher.parse(_c);
    }

    static void check_reset_counter() {
        eps::power_cycle_counters::bootup(emergency_sail_deployment);
    }
    static void init_hardware() {
        using namespace hal;
        using namespace std::chrono_literals;

        hal::Watchdog::disable();

        Serial0.init(38400);
        Serial0.redirect_stdio();
        Serial0.redirect_stderr();

        eps_a::MainTimer::init();

        iomap::PowerCycle::init(DigitalIO::Mode::OUTPUT);

        iomap::watchdog::Watchdog::init();

        Analog::InternalADC::init(Analog::InternalADC::Prescaler::DIV_128,
                                  Analog::InternalADC::Reference::AREF);

        LclCommander::init();
        LclCommander::off_all();

        Init::init();

        iomap::thermal_knives::PinSail::init(DigitalIO::Mode::OUTPUT);
        iomap::thermal_knives::PinSads::init(DigitalIO::Mode::OUTPUT);

        iomap::battery_controller::PinHeater::set();
        iomap::battery_controller::PinHeater::init(DigitalIO::Mode::OUTPUT);

        iomap::battery_controller::PinCharge::init(DigitalIO::Mode::OUTPUT);
        iomap::battery_controller::PinDischarge::init(DigitalIO::Mode::OUTPUT);

        iomap::SerialRx::init(DigitalIO::Mode::INPUT_PULLUP);

        // delay to make sure capacitances in power cycle circuit are
        // discharged. This is to prevent to fast power cycle.
        hal::sleep_for(150ms);
        iomap::PowerCycle::set();
    }

    static void init_obc_interface() {
        telemetry.init();
        telemetry.who_am_i = who_am_i;
        Obc::init(&telemetry);

        ObcWatchdog::kick();
    }
    static void emergency_sail_deployment() {
        LOG_FATAL("Opening SAIL");
        LOG_FATAL("Unimplemented!");
    }

    static void init() {
        init_hardware();
        LOG_INFO("Loaded EPS %c software rev. " GIT_REVISION
                 ", local state: " GIT_CHANGES,
                 side);

        check_reset_counter();
        init_obc_interface();

        LOG_INFO("EPS %c initialized.", side);
        sei();
    }

    using FullPowerCycle =
        eps::FullPowerCycle<typename iomap::PowerCycle, typename iomap::watchdog::Watchdog>;

    using LclCommander = eps::LclCommander<typename iomap::lcl::AllLcls>;
    using ObcWatchdog  = eps::ObcWatchdog<FullPowerCycle::perform>;


    using ThermalKnives =
        eps::ThermalKnives<typename iomap::thermal_knives::PinSail,
                           typename iomap::thermal_knives::PinSads>;  //

    using OverheatProtection = eps::OverheatProtection<LclCommander, 50>;

    struct Executor {
        template<typename Command>
        void invoke(Command& cmd, gsl::span<uint8_t> args) {
            cmd.invoke(args);
        }
    };

    using EPSACommandDispatcher =
        CommandDispatcher<Executor,                                     //
                          eps::commands::PowerCycle<FullPowerCycle>,    //
                          eps::commands::EnableLCL<LclCommander>,       //
                          eps::commands::DisableLCL<LclCommander>,      //
                          eps::commands::ThermalKnives<ThermalKnives>,  //
                          eps::commands::ObcWatchdog<ObcWatchdog>,      //
                          eps::commands::DisableOverheatProtection<OverheatProtection>  //
                          >;

    using Obc = drivers::ObcInterface<i2c_address,      //
                                      CommandCallback,  //
                                      EPSACommandDispatcher::max_command_length,  //
                                      eps_a::Telemetry>;

    static Telemetry telemetry;


    using TelemetryUpdater =
        eps_a::TelemetryUpdater<telemetry,
                                typename iomap::Mux,
                                hal::Analog::AnalogGPIO,
                                typename iomap::Mppt,
                                LclCommander,
                                typename iomap::battery_controller::TemperatureSensors>;


    static avr::Prescaler<33> timer_1second;
    static void each_33ms() {
        TelemetryUpdater::update_mppt();
        LclCommander::handle_overcurrent();

        if (timer_1second.expired()) {
            each_1sec();
        }
    }

    static avr::Prescaler<10> timer_10second;
    static void each_1sec() {
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            ThermalKnives::tick();
            ObcWatchdog::tick();
        }
        TelemetryUpdater::update_general();

        // TODO(ggajoch): use real temperature
        OverheatProtection::tick(0);

        if (timer_10second.expired()) {
            each_10sec();
        }
    }

    using BatteryManager =
        eps::BatteryManager<typename iomap::battery_controller::PinCharge,
                            typename iomap::battery_controller::PinDischarge,
                            typename iomap::battery_controller::PinHeater>;
    static BatteryManager battery_manager;

    static avr::Prescaler<6 * 30> timer_30min;
    static void each_10sec() {
        // TODO(ggajoch): replace with real battery voltage when calibrated
        // TODO(ggajoch): replace with real temperatures when they are measured
        battery_manager.tick(10, 0);

        if (timer_30min.expired()) {
            each_30min();
        }
    }

    static void each_30min() {
        LOG_INFO("Reset Safety Counter");
        eps::power_cycle_counters::reset();
    }

    static EPSACommandDispatcher dispatcher;

    static void TWI_ISR() {
        Obc::process_interrupt();
        dispatcher.dispatch();
    }

    static void run() {
        while (1) {
            if (eps_a::MainTimer::expired()) {
                each_33ms();
            }
        }
    }
};

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
Telemetry Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::telemetry;

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
typename Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::EPSACommandDispatcher
    Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::dispatcher;

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
avr::Prescaler<33> Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::timer_1second;

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
avr::Prescaler<10> Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::timer_10second;

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
avr::Prescaler<6 * 30> Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::timer_30min;

template<char side, uint8_t i2c_address, uint8_t who_am_i, typename Telemetry, typename iomap, typename Init>
typename Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::BatteryManager
    Epss<side, i2c_address, who_am_i, Telemetry, iomap, Init>::battery_manager;

#endif  // LIBS_EPS_EPS_H_
