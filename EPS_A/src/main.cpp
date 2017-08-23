#include "eps.h"

struct Settings {
    constexpr static char side           = 'A';
    constexpr static uint8_t i2c_address = 0x35;
    constexpr static uint8_t who_am_i    = 0x61;

    template<typename Eps>
    struct Specialisation {
        static void init() {
            eps::IOMap::Mux::init();
        }

        static void each_33ms() {
            Eps::TelemetryUpdater::update_mppt();
        }
    };
};

using Eps = Epss<Settings>;


ISR(TWI_vect) {
    Eps::TWI_ISR();
}


int main() {
    Eps::init();

    Eps::run();

    hal::libs::sim::stop_simulation();
}
