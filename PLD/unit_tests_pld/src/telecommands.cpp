#include "hardware/interface.h"
#include "telecommands/HouseKeeping.h"
#include "telemetry/telemetry.h"
extern "C" {
#include "unity_fixture.h"
};

TEST_GROUP(telecommands);
TEST_SETUP(telecommands) {
}
TEST_TEAR_DOWN(telecommands) {
}

TEST_GROUP_RUNNER(telecommands) {
    RUN_TEST_CASE(telecommands, HouseKeeping);
}


class MockHW : public pld::hardware::Interface {
 public:
    void init() override {
    }
    void standby() override {
    }
    uint16_t read_adc_and_change_channel(pld::hardware::AnalogChannel) override {
        return 0;
    }
    pld::Telemetry::Radfet read_radfet() override {
        return pld::Telemetry::Radfet();
    }
    void watchdog_kick() override {
    }
    void obc_interrupt_set() override {
    }
    void obc_interrupt_reset() override {
    }
};

pld::Telemetry telemetry;
MockHW hw;
pld::hardware::HardwareProvider hw_ptr;


TEST(telecommands, HouseKeeping) {
    hw_ptr = &hw;
    pld::telecommands::HouseKeeping hk(telemetry, hw_ptr);

    hk.work({});
}