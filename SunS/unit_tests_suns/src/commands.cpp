#include <hal/hal>
#include "unity.h"

#include "commands/measure.h"
#include "hardware/interface.h"
#include "telemetry/telemetry.h"

using namespace std;
using namespace hal;
using namespace hal::libs;

FIFO_data<uint8_t, 20> commands;

std::uint8_t expected_arg_0, expected_arg_1;

class MockHW : public suns::hardware::Interface {
 public:
    enum class MockHWCallStatus {
        not_invoked,
        invoked,
    };

    MockHWCallStatus MockHW_status;

    void init() override {
        TEST_FAIL_MESSAGE("init");
    }

    void measure() override {
        MockHW_status = MockHWCallStatus::invoked;
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

void test_commands_measure() {
    telemetry.init();
    hw.MockHW_status = MockHW::MockHWCallStatus::not_invoked;
    suns::commands::Measure().invoke(telemetry, hw, {});
    TEST_ASSERT_EQUAL(MockHW::MockHWCallStatus::invoked, hw.MockHW_status);
}

void test_commands() {
    UnityBegin("");
    RUN_TEST(test_commands_measure);
    UnityEnd();
}
