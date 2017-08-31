#include "MainTimer.h"

using namespace eps;

void MainTimer::init() {
    //     Timer1 - 1us tick
    hal::mcu::Timer1::init(hal::mcu::Timer1::Prescaler::DIV_8,
                           hal::mcu::Timer1::Mode::Normal);

    //     output compare to provide 33.333ms tick
    OCR1A = 33333;
}

bool MainTimer::expired() {
    if (TIFR1 & (1 << OCF1A)) {  // Timer1 compare flag
        TCNT1 = 0;               // reset Timer1 counter
        TIFR1 = (1 << OCF1A);    // clear Timer1 overflow flag

        return true;
    }
    return false;
}
