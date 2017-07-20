#include "AD7714_driver.h"
#include "AD7714.h"
#include "unity.h"

using namespace hal;
using namespace hal::libs;
using namespace pld::devices;

struct AD7714_mock {
    static bool init_called;
    static void init() {
        init_called = true;
    }

    static AD7714::Channels channel;
    static AD7714::Gain gain;

    static void setup_read(AD7714::Channels channel_,
                           AD7714::Gain gain_ = AD7714::Gain::GAIN_1) {
        channel = channel_;
        gain = gain_;
    }

    static uint16_t data_ready_counter;

    static bool data_ready() {
        if (data_ready_counter == 0) {
            return true;
        }
        data_ready_counter--;
        return false;
    }

    static uint24_t value;
    static uint24_t read_data_no_wait() {
        return value;
    }
};
bool AD7714_mock::init_called;
uint24_t AD7714_mock::value;
AD7714::Channels AD7714_mock::channel;
AD7714::Gain AD7714_mock::gain;
uint16_t AD7714_mock::data_ready_counter;

struct WDT {
    static std::uint16_t kicks;
    static void kick() {
        kicks++;
    }
};
std::uint16_t WDT::kicks;

using adc = AD7714::AD7714_driver<AD7714_mock, WDT>;


void test_AD7714_driver_init() {
    AD7714_mock::init_called = false;
    adc::init();
    TEST_ASSERT_TRUE(AD7714_mock::init_called);
}

void test_AD7714_driver_read_data() {
    for (uint32_t i = 0; i <= 0xFFFFFFul; i += 1000) {
        AD7714_mock::value = i;
        TEST_ASSERT_EQUAL(i, adc::read(AD7714::Channels::AIN_1_2));
    }

    AD7714_mock::value = 0xFFFFFFul;
    TEST_ASSERT_EQUAL(0xFFFFFF, adc::read(AD7714::Channels::AIN_1_2));
}

void test_AD7714_driver_read_channel() {
    auto test = [](AD7714::Channels ch) {
        adc::read(ch);
        TEST_ASSERT_EQUAL(ch, AD7714_mock::channel);
    };

    test(AD7714::Channels::AIN_1_6);
    test(AD7714::Channels::AIN_2_6);
    test(AD7714::Channels::AIN_3_6);
    test(AD7714::Channels::AIN_4_6);
    test(AD7714::Channels::AIN_1_2);
    test(AD7714::Channels::AIN_3_4);
    test(AD7714::Channels::AIN_5_6);
    test(AD7714::Channels::TEST);
}

void test_AD7714_driver_read_gains() {
    auto test = [](AD7714::Gain gain) {
        adc::read(AD7714::Channels::TEST, gain);
        TEST_ASSERT_EQUAL(gain, AD7714_mock::gain);
    };

    test(AD7714::Gain::GAIN_1);
    test(AD7714::Gain::GAIN_2);
    test(AD7714::Gain::GAIN_4);
    test(AD7714::Gain::GAIN_8);
    test(AD7714::Gain::GAIN_16);
    test(AD7714::Gain::GAIN_32);
    test(AD7714::Gain::GAIN_64);
    test(AD7714::Gain::GAIN_128);
}

void test_AD7714_driver_waits_for_data_ready() {
    auto test = [](uint16_t val) {
        AD7714_mock::data_ready_counter = val;
        adc::read(AD7714::Channels::TEST);
        TEST_ASSERT_EQUAL_UINT16(0, AD7714_mock::data_ready_counter);
    };

    test(0);
    test(1);
    test(10);
    test(1000);
    test(5000);
}

void test_AD7714_driver_timeout() {
    auto test = [](uint16_t ticks) {
        AD7714_mock::data_ready_counter = ticks;
        adc::read(AD7714::Channels::TEST);
        TEST_ASSERT_EQUAL_UINT16(ticks-8000, AD7714_mock::data_ready_counter);
    };

    test(10000);
    test(15000);
}

void test_AD7714_driver_watchdog() {
    auto test = [](uint16_t ticks) {
        WDT::kicks = 0;
        AD7714_mock::data_ready_counter = ticks;
        adc::read(AD7714::Channels::TEST);
        TEST_ASSERT_EQUAL_UINT16(ticks, WDT::kicks);
    };

    test(0);
    test(1);
    test(10);
    test(1000);
    test(5000);
}

void test_AD7714_driver_watchdog_timeout() {
    auto test = [](uint16_t ticks) {
        WDT::kicks = 0;
        AD7714_mock::data_ready_counter = ticks;
        adc::read(AD7714::Channels::TEST);
        TEST_ASSERT_EQUAL_UINT16(8000, WDT::kicks);
    };

    test(9000);
    test(15000);
}

void test_AD7714_driver() {
    UnityBegin("");
    RUN_TEST(test_AD7714_driver_init);
    RUN_TEST(test_AD7714_driver_read_data);
    RUN_TEST(test_AD7714_driver_read_channel);
    RUN_TEST(test_AD7714_driver_read_gains);
    RUN_TEST(test_AD7714_driver_waits_for_data_ready);
    RUN_TEST(test_AD7714_driver_timeout);
    RUN_TEST(test_AD7714_driver_watchdog);
    RUN_TEST(test_AD7714_driver_watchdog_timeout);
    UnityEnd();
}
