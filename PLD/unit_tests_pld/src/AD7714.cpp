#include "AD7714.h"
#include "unity.h"

using namespace hal;
using namespace hal::libs;
using namespace pld::devices;

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
    static std::array<uint8_t, 20> reads_buffer;
    static hal::libs::FIFO_data<uint8_t, 20> write_buffer;
    static uint8_t ptr;

    static void reset() {
        ptr = 0;
        write_buffer.flush();
    }

    static void schedule_reads(std::initializer_list<uint8_t> buf) {
        std::copy(buf.begin(), buf.end(), reads_buffer.begin());
    }

    static void check_writes(std::initializer_list<uint8_t> expects) {
        TEST_ASSERT_EQUAL_UINT8(expects.size(), write_buffer.getLength());

        for (auto now : expects) {
            auto top = write_buffer.get();
            ptr--;
            TEST_ASSERT_EQUAL_HEX8(now, top);
        }
        TEST_ASSERT_EQUAL(0, ptr);
    }

    static void write(gsl::span<const uint8_t> out) {
        for (auto x : out) {
            write_buffer.append(x);
            ptr++;
        }
    }

    static void read(gsl::span<uint8_t> in) {
        for (auto& x : in) {
            x = reads_buffer[ptr++];
            write_buffer.append(0);
        }
    }
};

std::array<uint8_t, 20> spi::reads_buffer;
hal::libs::FIFO_data<uint8_t, 20> spi::write_buffer;
uint8_t spi::ptr = 0;

using ad7714 = pld::devices::AD7714::AD7714<spi, pinIO>;

void test_AD7714_init() {
    pinIO::was_initialised = false;

    ad7714::init();

    TEST_ASSERT_TRUE(pinIO::was_initialised);
}

void test_AD7714_data_ready() {
    pinIO::value = true;
    TEST_ASSERT_FALSE(ad7714::data_ready());
    pinIO::value = false;
    TEST_ASSERT_TRUE(ad7714::data_ready());
    TEST_ASSERT_TRUE(ad7714::data_ready());
    pinIO::value = true;
    TEST_ASSERT_FALSE(ad7714::data_ready());
}

void check_channel_change(AD7714::Channels channel,
                          uint8_t channel_nr,
                          AD7714::Gain gain,
                          uint8_t gain_nr) {
    ad7714::change_channel(channel, gain);
    spi::check_writes(
        {0x20 | channel_nr,  // comm register; write to filter high; channel
                             // channel_nr
         0xEF,               // unipolar; 24b; curr. boost; filter 4000
         0x30 | channel_nr,  // comm register; write to filter low; channel
                             // channel_nr
         0xA0,               // filter 4000
         0x10 | channel_nr,  // comm register; write to mode; channel channel_nr
         0x20 | gain_nr});   // self calib mode
}

void test_AD7714_change_channel() {
    check_channel_change(AD7714::Channels::AIN_1_6, 0, AD7714::Gain::GAIN_1, 0);
    check_channel_change(
        AD7714::Channels::AIN_2_6, 1, AD7714::Gain::GAIN_128, 0b11100);
    check_channel_change(
        AD7714::Channels::AIN_3_6, 2, AD7714::Gain::GAIN_32, 0b10100);
    check_channel_change(
        AD7714::Channels::AIN_4_6, 3, AD7714::Gain::GAIN_64, 0b11000);

    check_channel_change(
        AD7714::Channels::AIN_1_2, 4, AD7714::Gain::GAIN_2, 0b00100);
    check_channel_change(
        AD7714::Channels::AIN_3_4, 5, AD7714::Gain::GAIN_4, 0b01000);
    check_channel_change(
        AD7714::Channels::AIN_5_6, 6, AD7714::Gain::GAIN_8, 0b01100);
    check_channel_change(AD7714::Channels::TEST, 7, AD7714::Gain::GAIN_16, 0b10000);
}

void test_AD7714_read_data_case(AD7714::Channels channel,
                                uint8_t a,
                                uint8_t b,
                                uint8_t c,
                                uint32_t expected) {
    ad7714::change_channel(channel, AD7714::Gain::GAIN_1);
    spi::reset();

    spi::schedule_reads({0,  // comm register
                         a,
                         b,
                         c});

    TEST_ASSERT_EQUAL_HEX32(expected, ad7714::read_data_no_wait());

    spi::check_writes({0x58 | num(channel),  // read from data register
                       0,
                       0,
                       0});
}

void test_AD7714_read_data() {
    test_AD7714_read_data_case(
        AD7714::Channels::AIN_1_6, 0x12, 0x34, 0x56, 0x123456);
    test_AD7714_read_data_case(
        AD7714::Channels::AIN_2_6, 0x43, 0x8A, 0x4F, 0x438A4F);
    test_AD7714_read_data_case(
        AD7714::Channels::AIN_1_2, 0xFF, 0xFF, 0xFF, 0xFFFFFF);
    test_AD7714_read_data_case(
        AD7714::Channels::AIN_3_4, 0x00, 0x00, 0x00, 0x000000);
}

void test_AD7714_channel_and_read() {
    spi::schedule_reads({0, 0, 0, 0, 0, 0, 0, 0x58, 0xA7, 0xFE});

    ad7714::change_channel(AD7714::Channels::AIN_4_6, AD7714::Gain::GAIN_32);

    TEST_ASSERT_EQUAL_HEX32(0x58A7FE, ad7714::read_data_no_wait());

    spi::check_writes({0x23, 0xEF, 0x33, 0xA0, 0x13, 0x34, 0x5B, 0, 0, 0});
}

void test_AD7714() {
    UnityBegin("");
    RUN_TEST(test_AD7714_init);
    RUN_TEST(test_AD7714_data_ready);
    RUN_TEST(test_AD7714_change_channel);
    RUN_TEST(test_AD7714_read_data);
    RUN_TEST(test_AD7714_channel_and_read);
    UnityEnd();
}
