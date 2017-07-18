#include "AD7714_driver.h"
#include "AD7714.h"
#include "unity.h"

using namespace hal;
using namespace hal::libs;
using namespace pld::devices;

struct AD7714_mock {
    enum class MockCall { None, Init, SetupRead, DataReady, Read, Duplicated };
    static MockCall call;
    static void called(MockCall caller) {
        if (call == MockCall::None) {
            call = caller;
        } else if (call == caller) {
            call = MockCall::Duplicated;
        }
    }


    static void init() {
        called(MockCall::Init);
    }

    static AD7714::Channels channel;

    static void
    setup_read(AD7714::Channels ch, AD7714::Gain gain = AD7714::Gain::GAIN_1) {
        channel = ch;
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
AD7714_mock::MockCall AD7714_mock::call;
uint24_t AD7714_mock::value;
AD7714::Channels AD7714_mock::channel;
uint16_t AD7714_mock::data_ready_counter;

using adc = AD7714::AD7714_driver<AD7714_mock>;


void test_AD7714_driver_init() {
    adc::init();
    TEST_ASSERT_EQUAL(AD7714_mock::MockCall::Init, AD7714_mock::call);
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
    AD7714_mock::data_ready_counter = 10000;
    adc::read(AD7714::Channels::TEST);
    TEST_ASSERT_EQUAL_UINT16(2000, AD7714_mock::data_ready_counter);

    AD7714_mock::data_ready_counter = 15000;
    adc::read(AD7714::Channels::TEST);
    TEST_ASSERT_EQUAL_UINT16(7000, AD7714_mock::data_ready_counter);
}

void test_AD7714_driver() {
    UnityBegin("");
    RUN_TEST(test_AD7714_driver_init);
    RUN_TEST(test_AD7714_driver_read_data);
    RUN_TEST(test_AD7714_driver_read_channel);
    RUN_TEST(test_AD7714_driver_waits_for_data_ready);
    RUN_TEST(test_AD7714_driver_timeout);
    UnityEnd();
}
