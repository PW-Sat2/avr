#ifndef EPS_B_EPS_LIBS_MAINTIMER_MAINTIMER_H_
#define EPS_B_EPS_LIBS_MAINTIMER_MAINTIMER_H_

#include <hal/hal>

namespace eps {

/*!
 * Main EPS_A timer. Uses Timer1 in CTC_OCR1A mode.
 * This module provides 33.333ms tick.
 */
struct MainTimer {
    /*!
     * Initialises timer.
     */
    static void init();

    /*!
     * Check if timer expired. If so, reset the timer.
     * @return True if timer expired.
     */
    static bool expired();
};


};  // namespace eps

#endif  // EPS_B_EPS_LIBS_MAINTIMER_MAINTIMER_H_
