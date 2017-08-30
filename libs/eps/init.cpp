#include <MainTimer.h>
#include <PowerCycleCounter.h>

#include "defs.h"

void init_hardware() {
    using namespace avr;
    using namespace eps;
    using namespace hal;
    using namespace std::chrono_literals;

    hal::Watchdog::disable();

    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    eps::MainTimer::init();

    eps::IOMap::PowerCycle::init(DigitalIO::Mode::OUTPUT);

    IOMap::Watchdog::init();

    Analog::InternalADC::init(Analog::InternalADC::Prescaler::DIV_128,
                              Analog::InternalADC::Reference::AREF);

    Eps::LclCommander::init();
    Eps::LclCommander::off_all();

    ControllerSpecialisation::init();

    IOMap::ThermalKnives::Sail::init(DigitalIO::Mode::OUTPUT);
    IOMap::ThermalKnives::Sads::init(DigitalIO::Mode::OUTPUT);

    IOMap::Battery::Heater::set();
    IOMap::Battery::Heater::init(DigitalIO::Mode::OUTPUT);

    IOMap::Battery::Charge::init(DigitalIO::Mode::OUTPUT);
    IOMap::Battery::Discharge::init(DigitalIO::Mode::OUTPUT);

    IOMap::SerialRx::init(DigitalIO::Mode::INPUT_PULLUP);

    // delay to make sure capacitances in power cycle circuit are
    // discharged. Epss is to prevent to fast power cycle.
    hal::sleep_for(150ms);
    IOMap::PowerCycle::set();
}

void init_obc_interface();

void avr::Eps::init() {
    init_hardware();
    LOG_INFO("Loaded EPS_%c software rev. " GIT_REVISION
             ", local state: " GIT_CHANGES,
             ControllerSpecialisation::side);

    eps::power_cycle_counters::bootup([]() {
        LOG_FATAL("Opening SAIL");
        LclCommander::on(num(IOMap::LclId::ThermalKnives));
        ThermalKnives::burn(ThermalKnives::Types::Sail);
    });

    init_obc_interface();

    ObcWatchdog::kick();

    LOG_INFO("EPS_%c initialized.", ControllerSpecialisation::side);
    sei();
}
