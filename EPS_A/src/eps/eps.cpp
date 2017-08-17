#include "eps.h"

#include "MainTimer.h"
#include "main.h"

using namespace hal;
using namespace eps_a;
using namespace std::chrono_literals;

void eps_a::init_hardware() {
    hal::Watchdog::disable();

    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    eps_a::MainTimer::init();

    iomap::PowerCycle::init(DigitalIO::Mode::OUTPUT);
    iomap::PowerCycle::reset();

    iomap::watchdog::Watchdog::init();

    Analog::InternalADC::init(Analog::InternalADC::Prescaler::DIV_128,
                              Analog::InternalADC::Reference::AREF);

    iomap::mux::Mux::init();

    eps::LclCommander<iomap::lcl::AllLcls>::init();
    eps::LclCommander<iomap::lcl::AllLcls>::off_all();

    iomap::thermal_knives::PinSail::init(DigitalIO::Mode::OUTPUT);
    iomap::thermal_knives::PinSads::init(DigitalIO::Mode::OUTPUT);

    iomap::battery_controller::PinHeater::set();
    iomap::battery_controller::PinHeater::init(DigitalIO::Mode::OUTPUT);

    iomap::battery_controller::PinCharge::init(DigitalIO::Mode::OUTPUT);
    iomap::battery_controller::PinCharge::reset();
    iomap::battery_controller::PinDischarge::init(DigitalIO::Mode::OUTPUT);
    iomap::battery_controller::PinDischarge::reset();

    iomap::SerialRx::init(DigitalIO::Mode::INPUT_PULLUP);

    // delay to make sure capacitances in power cycle circuit are discharged.
    // This is to prevent to fast power cycle.
    hal::sleep_for(150ms);
    iomap::PowerCycle::set();
}

void eps_a::init_obc_interface() {
    telemetry.init();
    telemetry.who_am_i = 0x61;
    Obc::init(&telemetry);

    ::ObcWatchdog::kick();
}


void eps_a::emergency_sail_deployment() {
    LOG_FATAL("Opening SAIL");
    ::LclCommander::on(num(iomap::lcl::LclId::TkMain));
    ThermalKnives::burn(ThermalKnives::Types::Sail);
}
