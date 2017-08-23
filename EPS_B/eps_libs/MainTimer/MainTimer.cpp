#include "MainTimer.h"

using namespace eps;

void MainTimer::init() {
    // Timer1 - 1us tick
    //    hal::Timer1::init(hal::Timer1::Prescaler::DIV_8,
    //    hal::Timer1::Mode::normal);

    // output compare to provide 33.333ms tick
    //    OCR1A = 33333;
}

bool MainTimer::expired() {
    //    if (TIFR & (1 << OCF1A)) {  // Timer1 compare flag
    //        TCNT1 = 0;              // reset Timer1 counter
    //        TIFR  = (1 << OCF1A);   // clear Timer1 overflow flag

    return true;
    //    }
    //    return false;
}
