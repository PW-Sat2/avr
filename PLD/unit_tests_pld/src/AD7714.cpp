#include "AD7714.h"
#include "unity.h"

using namespace hal;
using namespace hal::libs;
using namespace pld::devices;

struct pinIO {
    static bool was_initialised, value;
    static uint16_t ready_counter;

    static void init(const hal::DigitalIO::Mode mode) {
        TEST_ASSERT_EQUAL(mode, DigitalIO::Mode::INPUT_PULLUP);
        was_initialised = true;
    }


    static bool read() {
        if (ready_counter == 0) {
            return value;
        }
        ready_counter--;
        return true;
    }
};

bool pinIO::was_initialised, pinIO::value;
uint16_t pinIO::ready_counter = 0;


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

void test_AD7714_waits_for_data_ready() {
    pinIO::ready_counter = 100;
    pinIO::value         = false;

    ad7714::read_data(AD7714::Channels::TEST);
    TEST_ASSERT_EQUAL_UINT16(0, pinIO::ready_counter);
    spi::reset();

    pinIO::ready_counter = 2000;
    pinIO::value         = false;

    ad7714::read_data(AD7714::Channels::TEST);
    TEST_ASSERT_EQUAL_UINT16(0, pinIO::ready_counter);
    spi::reset();
}

void check_test_case(AD7714::Channels channel,
                     uint8_t channel_nr,
                     AD7714::Gain gain,
                     uint8_t gain_nr,
                     uint8_t byte_1,
                     uint8_t byte_2,
                     uint8_t byte_3,
                     uint32_t expected_word) {
    spi::schedule_reads({0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,  // comm register
                         byte_1,
                         byte_2,
                         byte_3});

    TEST_ASSERT_EQUAL_HEX32(expected_word, ad7714::read_data(channel, gain));

    spi::check_writes(
        {0x20 | channel_nr,  // comm register; write to filter high;
         0xEF,               // unipolar; 24b; curr. boost; filter 4000
         0x30 | channel_nr,  // comm register; write to filter low;
         0xA0,               // filter 4000
         0x10 | channel_nr,  // comm register; write to mode; channel channel_nr
         0x20 | gain_nr,     // self calib mode
         0x58 | num(channel),  // read from data register
         0,
         0,
         0});
}

void test_AD7714_data_readout() {
    check_test_case(AD7714::Channels::AIN_1_6,
                    0,
                    AD7714::Gain::GAIN_1,
                    0b00000,
                    0x12,
                    0x34,
                    0x56,
                    0x123456);

    check_test_case(AD7714::Channels::AIN_2_6,
                    1,
                    AD7714::Gain::GAIN_128,
                    0b11100,
                    0x43,
                    0x8A,
                    0x4F,
                    0x438A4F);

    check_test_case(AD7714::Channels::AIN_3_6,
                    2,
                    AD7714::Gain::GAIN_32,
                    0b10100,
                    0xFF,
                    0xFF,
                    0xFF,
                    0xFFFFFF);

    check_test_case(AD7714::Channels::AIN_4_6,
                    3,
                    AD7714::Gain::GAIN_64,
                    0b11000,
                    0x00,
                    0x00,
                    0x00,
                    0x000000);

    check_test_case(AD7714::Channels::AIN_1_2,
                    4,
                    AD7714::Gain::GAIN_2,
                    0b00100,
                    0x74,
                    0x5A,
                    0xFE,
                    0x745AFE);

    check_test_case(AD7714::Channels::AIN_3_4,
                    5,
                    AD7714::Gain::GAIN_4,
                    0b01000,
                    0xDE,
                    0xAD,
                    0x55,
                    0xDEAD55);

    check_test_case(AD7714::Channels::AIN_5_6,
                    6,
                    AD7714::Gain::GAIN_8,
                    0b01100,
                    0xBE,
                    0xEF,
                    0xA1,
                    0xBEEFA1);

    check_test_case(AD7714::Channels::TEST,
                    7,
                    AD7714::Gain::GAIN_16,
                    0b010000,
                    0x0F,
                    0xF1,
                    0xCE,
                    0x0FF1CE);
}

void test_AD7714_explicit_case() {
    spi::schedule_reads({0, 0, 0, 0, 0, 0, 0, 0x58, 0xA7, 0xFE});

    pinIO::value = false;
    TEST_ASSERT_EQUAL_HEX32(
        0x58A7FE,
        ad7714::read_data(AD7714::Channels::AIN_4_6, AD7714::Gain::GAIN_32));

    spi::check_writes({0x23, 0xEF, 0x33, 0xA0, 0x13, 0x34, 0x5B, 0, 0, 0});
}

void test_AD7714() {
    UnityBegin("");
    RUN_TEST(test_AD7714_init);
    RUN_TEST(test_AD7714_waits_for_data_ready);
    RUN_TEST(test_AD7714_data_readout);
    RUN_TEST(test_AD7714_explicit_case);
    UnityEnd();
}
