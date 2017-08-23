#include "eps.h"

struct Settings {
    constexpr static char side           = 'B';
    constexpr static uint8_t i2c_address = 0x36;
    constexpr static uint8_t who_am_i    = 0x9D;

    template<typename Eps>
    struct Specialisation {
        static void init() {
        }

        static void each_33ms() {
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
