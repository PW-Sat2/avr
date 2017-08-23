#ifndef LIBS_EPS_EPS_H_
#define LIBS_EPS_EPS_H_

#include <BatteryManager.h>
#include <MainTimer.h>
#include <PowerCycleCounter.h>
#include <TelemetryUpdater.h>
#include <telemetry.h>
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
#include "ThermalKnives.h"

#include "Prescaler.h"

#include "IOMap.h"


template<typename Settings>
struct Epss {
    using This      = Epss<Settings>;
    using IOMap     = eps::IOMap;
    using Telemetry = eps::Telemetry;
    using MainTimer = eps::MainTimer;
    using Specific  = typename Settings::template Specialisation<This>;

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

        MainTimer::init();

        IOMap::PowerCycle::init(DigitalIO::Mode::OUTPUT);

        IOMap::watchdog::Watchdog::init();

        Analog::InternalADC::init(Analog::InternalADC::Prescaler::DIV_128,
                                  Analog::InternalADC::Reference::AREF);

        LclCommander::init();
        LclCommander::off_all();

        Specific::init();

        IOMap::thermal_knives::PinSail::init(DigitalIO::Mode::OUTPUT);
        IOMap::thermal_knives::PinSads::init(DigitalIO::Mode::OUTPUT);

        IOMap::battery_controller::PinHeater::set();
        IOMap::battery_controller::PinHeater::init(DigitalIO::Mode::OUTPUT);

        IOMap::battery_controller::PinCharge::init(DigitalIO::Mode::OUTPUT);
        IOMap::battery_controller::PinDischarge::init(DigitalIO::Mode::OUTPUT);

        IOMap::SerialRx::init(DigitalIO::Mode::INPUT_PULLUP);

        // delay to make sure capacitances in power cycle circuit are
        // discharged. This is to prevent to fast power cycle.
        hal::sleep_for(150ms);
        IOMap::PowerCycle::set();
    }

    static void init_obc_interface() {
        telemetry.init();
        telemetry.who_am_i = Settings::who_am_i;
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
                 Settings::side);

        check_reset_counter();
        init_obc_interface();

        LOG_INFO("EPS %c initialized.", Settings::side);
        sei();
    }

    using FullPowerCycle =
        eps::FullPowerCycle<typename IOMap::PowerCycle, typename IOMap::watchdog::Watchdog>;

    using LclCommander = eps::LclCommander<typename IOMap::lcl::AllLcls>;
    using ObcWatchdog  = eps::ObcWatchdog<FullPowerCycle::perform>;


    using ThermalKnives =
        eps::ThermalKnives<typename IOMap::thermal_knives::PinSail,
                           typename IOMap::thermal_knives::PinSads>;  //

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

    using Obc = drivers::ObcInterface<Settings::i2c_address,  //
                                      CommandCallback,        //
                                      EPSACommandDispatcher::max_command_length,  //
                                      Telemetry>;

    static Telemetry telemetry;


    using TelemetryUpdater =
        eps::TelemetryUpdater<telemetry, hal::Analog::AnalogGPIO, This>;


    static avr::Prescaler<33> timer_1second;
    static void each_33ms() {
        Specific::each_33ms();

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
        eps::BatteryManager<typename IOMap::battery_controller::PinCharge,
                            typename IOMap::battery_controller::PinDischarge,
                            typename IOMap::battery_controller::PinHeater>;
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
            if (MainTimer::expired()) {
                each_33ms();
            }
        }
    }
};

template<typename Settings>
eps::Telemetry Epss<Settings>::telemetry;

template<typename Settings>
typename Epss<Settings>::EPSACommandDispatcher Epss<Settings>::dispatcher;

template<typename Settings>
avr::Prescaler<33> Epss<Settings>::timer_1second;

template<typename Settings>
avr::Prescaler<10> Epss<Settings>::timer_10second;

template<typename Settings>
avr::Prescaler<6 * 30> Epss<Settings>::timer_30min;

template<typename Settings>
typename Epss<Settings>::BatteryManager Epss<Settings>::battery_manager;

#endif  // LIBS_EPS_EPS_H_
