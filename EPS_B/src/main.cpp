#include "eps.h"

struct Settings {
    template<typename Eps>
    struct Specialisation {
        static void init() {
        }

        static void each_33ms() {
        }
    };
};

int main() {
    avr::Eps::init();

    avr::Eps::run();

    hal::libs::sim::stop_simulation();
}
