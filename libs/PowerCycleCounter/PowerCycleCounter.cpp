#include "PowerCycleCounter.h"
#include "EepromCounter.h"

using namespace avr;
using namespace avr::power_cycle_counters;

EepromCounter<uint16_t>::type EEMEM var_counter_safety{0, 0, 0, 0, 0};
EepromCounter<uint16_t>::type EEMEM var_counter_all{0, 0, 0, 0, 0};

EepromCounter<uint16_t> counter_safety{var_counter_safety},
    counter_all{var_counter_all};


void power_cycle_counters::bootup(void (*emergency_open_sail)()) {
    auto all = counter_all.get();
    LOG_INFO("Bootup nr %u\n", all);

    auto safety = counter_safety.get();
    LOG_INFO("Safety counter %u\n", safety);

    if (safety > 192) {
        LOG_FATAL("Emeregency sail deployment!\n");
        emergency_open_sail();
    } else {
        counter_safety.increment();
    }

    counter_all.increment();
}


Counter power_cycle_counters::get() {
    Counter counter;

    counter.all    = counter_all.get();
    counter.safety = counter_safety.get();

    return counter;
}

void power_cycle_counters::reset() {
    counter_safety.reset();
}
