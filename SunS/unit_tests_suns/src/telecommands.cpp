#include <hal/hal>
#include "unity.h"

#include "hardware/interface.h"
#include "telemetry/telemetry.h"

#include "telecommands/measure.h"

using namespace std;
using namespace hal;
using namespace hal::libs;

FIFO_data<uint8_t, 20> commands;

class MockHW : public suns::hardware::Interface {
 public:
    void init() override {
        TEST_FAIL_MESSAGE("init");
    }

    suns::Telemetry::ALS als_measure(uint8_t gain, uint8_t itime) override {
        TEST_FAIL_MESSAGE("als measure");
    }

    void watchdog_kick() override {
        TEST_FAIL_MESSAGE("watchdog kicked");
    }

    void obc_interrupt_set() override {
        TEST_FAIL_MESSAGE("interrupt set");
    }

    void obc_interrupt_reset() override {
        TEST_FAIL_MESSAGE("interrupt reset");
    }
};

suns::Telemetry telemetry;
MockHW hw;
suns::hardware::HardwareProvider hw_ptr;

using namespace suns::hardware;

void test_telecommands() {
    UnityBegin("");
    UnityEnd();
}
