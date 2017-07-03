#include "unity.h"
#include "AD7714.h"

using namespace hal;
using namespace hal::libs;
using namespace ::devices;

struct pinIO {
    static bool was_initialised, value;

    static void init(const hal::DigitalIO::Mode mode) {
        TEST_ASSERT_EQUAL(mode, DigitalIO::Mode::INPUT_PULLUP);
        was_initialised = true;
    }

    static bool read() {
        return value;
    }
};
bool pinIO::was_initialised, pinIO::value;



struct spi {
    struct MockElement {
        uint8_t expected;
        uint8_t out;
    };

    static std::array<MockElement, 10> data;
    static uint8_t length, ptr;

    static void schedule(std::initializer_list<const MockElement> mock_data) {
        length = mock_data.size();
        std::copy(mock_data.begin(), mock_data.end(), data.begin());
        ptr = 0;
    }

    static void write(gsl::span<const uint8_t> out) {
        for(auto x : out) {
            TEST_ASSERT_EQUAL_HEX8(data[ptr++].expected, x);
        }
    }

    static void read(gsl::span<uint8_t> in) {
        for(auto& x : in) {
            TEST_ASSERT_EQUAL_HEX8(0, data[ptr].expected);
            x = data[ptr++].out;
        }
    }

    static void check() {
        TEST_ASSERT_EQUAL_UINT8(length, ptr);
    }
};
std::array<spi::MockElement, 10> spi::data;
uint8_t spi::length, spi::ptr;

using ad7714 = ::devices::AD7714::AD7714<spi, pinIO>;

void test_AD7714_init() {
    pinIO::was_initialised = false;

    ad7714::init();

    TEST_ASSERT_TRUE(pinIO::was_initialised);
}

void test_AD7714_data_ready() {
    pinIO::value = true;
    TEST_ASSERT_FALSE(ad7714::data_is_ready());
    pinIO::value = false;
    TEST_ASSERT_TRUE(ad7714::data_is_ready());
    TEST_ASSERT_TRUE(ad7714::data_is_ready());
    pinIO::value = true;
    TEST_ASSERT_FALSE(ad7714::data_is_ready());
}

void test_AD7714_change_channel() {
    spi::schedule({{0x20, 0},  // comm register; write to filter high; channel 0
                                         {0xEF, 0}, // unipolar; 24b; curr. boost; filter 4000
                                         {0x38, 0},  // comm register; write to filter low; channel 0
                                         {0xA0, 0}});  // filter 4000

    ad7714::change_channel(AD7714::Channels::AIN1_CH);

    spi::check();
}

void test_AD7714_read_data() {
    // spi::schedule(std::array<uint8_t, 4>{0, 0, 0, 0}, std::array<uint8_t, 4>{0, 0, 0, 0});
    TEST_ASSERT_EQUAL_UINT32(0, ad7714::read_data_no_wait());
}

void test_AD7714() {
    UnityBegin("");
    RUN_TEST(test_AD7714_init);
    RUN_TEST(test_AD7714_data_ready);
    RUN_TEST(test_AD7714_change_channel);
    RUN_TEST(test_AD7714_read_data);
    UnityEnd();
}
