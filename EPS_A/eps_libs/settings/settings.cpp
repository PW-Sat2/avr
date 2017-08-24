#include "IOMap.h"

#include "eps.h"
#include "settings.h"

using namespace avr;

void ControllerSpecialisation::init() {
    eps::IOMap::Mux::init();
}

void ControllerSpecialisation::each_33ms() {
    Eps::TelemetryUpdater::update_mppt();
}
