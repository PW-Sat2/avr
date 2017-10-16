#include <MainTimer.h>
#include <PowerCycleCounter.h>

#include "defs.h"

template<typename T>
struct PinInitExec {
    static void run() {
        T::init(hal::DigitalIO::Mode::INPUT_PULLUP);
    }
};

void init_hardware() {
    using namespace avr;
    using namespace eps;
    using namespace hal;
    using namespace std::chrono_literals;

    hal::Watchdog::disable();

    // Serial init to max possible baudrate
    UBRR0H = 0;
    UBRR0L = 0;

    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    Serial0.redirect_stdio();
    Serial0.redirect_stderr();

    eps::MainTimer::init();

    eps::IOMap::PowerCycle::init(DigitalIO::Mode::OUTPUT);

    IOMap::Watchdog::init();

    Analog::InternalADC::init(Analog::InternalADC::Prescaler::DIV_8,
                              Analog::InternalADC::Reference::AREF);

    Eps::LclCommander::init();
    Eps::LclCommander::off_all();

    ControllerSpecialisation::init();

    IOMap::ThermalKnives::Sail::init(DigitalIO::Mode::OUTPUT);
    IOMap::ThermalKnives::Sads::init(DigitalIO::Mode::OUTPUT);

    IOMap::Battery::Heater::init();

    IOMap::Battery::Charge::init(DigitalIO::Mode::OUTPUT);
    IOMap::Battery::Discharge::init(DigitalIO::Mode::OUTPUT);

    hal::libs::for_each_tuple_type<PinInitExec, IOMap::PinsAsInputPullup>();

    // delay to make sure capacitances in power cycle circuit are
    // discharged. This is to prevent too frequent power cycle.
    hal::sleep_for(150ms);
    IOMap::PowerCycle::set();
}

void init_obc_interface();

void avr::Eps::init() {
    init_hardware();
    LOG_ALWAYS("Loaded EPS_%c software rev. " GIT_REVISION
               ", local state: " GIT_CHANGES,
               ControllerSpecialisation::side);

    eps::power_cycle_counters::bootup([]() {
        LOG_FATAL("Opening SAIL");
        OverheatProtection::disable();
        LclCommander::on(num(IOMap::LclId::ThermalKnives));
        ThermalKnives::burn(ThermalKnives::Types::Sail);
    });

    init_obc_interface();

    ObcWatchdog::kick();

    LOG_INFO("EPS_%c initialized.", ControllerSpecialisation::side);
    sei();
}
