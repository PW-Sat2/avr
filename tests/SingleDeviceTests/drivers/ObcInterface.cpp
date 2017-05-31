#include <hal/hal>
#include <unity.h>
#include "hal/board.h"
#include "ObcInterface/ObcInterface.h"

using namespace hal;

hal::DigitalIO::GPIO<hal::bsp::pins::D2> d2;
hal::DigitalIO::GPIO<hal::bsp::pins::D3> d3;

hal::I2C::Software i2c{d2, d3};

struct Data {
    uint8_t error;
    libs::array<uint8_t, 100> data;
} data;

libs::array<uint8_t, 10> data_arr;
libs::span<uint8_t> data_span;


void CommandCallback(libs::span<uint8_t> param) {
    for(uint8_t i = 0; i < param.size(); ++i) {
        data_arr[i] = param[i];
    }
    data_span = libs::make_span(data_arr.data(), param.size());
}

::drivers::ObcInterface<0x1A, CommandCallback, 4, Data> obc;

ISR(TWI_vect) {
    obc.process_interrupt();
}

void compare_spans(libs::span<uint8_t> expected, libs::span<uint8_t> actual) {
    TEST_ASSERT_EQUAL_INT(expected.size(), actual.size());
    for(size_t i = 0; i < expected.size(); ++i) {
        TEST_ASSERT_EQUAL_INT(expected[i], actual[i]);
    }
}

// ----------------  tests commands -------------------

void test_cmd_empty() {
    i2c.write(0x1A, {});
    compare_spans({}, data_span);
}

void test_cmd_short() {
    libs::array<uint8_t, 1> write = {0x00};
    i2c.write(0x1A, write);
    compare_spans(write, data_span);
}

void test_cmd_longer() {
    libs::array<uint8_t, 3> write = {131, 75, 84};
    i2c.write(0x1A, write);
    compare_spans(write, data_span);
}

void test_cmd_full() {
    libs::array<uint8_t, 4> write = {134, 65, 180, 189};
    i2c.write(0x1A, write);
    compare_spans(write, data_span);
}

void test_cmd_overflow() {
    libs::array<uint8_t, 5> write = {101, 45, 230, 200, 10};
    i2c.write(0x1A, write);

    compare_spans(libs::make_span(write).subspan(0, 4), data_span);
}

void test_cmd_large_overflow() {
    libs::array<uint8_t, 255> write;
    for (size_t i = 0; i < write.size(); ++i) {
        write[i] = i;
    }
    i2c.write(0x1A, write);

    compare_spans(libs::make_span(write).subspan(0, 4), data_span);
}

// ----------------  tests reads  -------------------

void test_read_only_error() {
    for(uint16_t i = 0; i <= 0xFF; ++i) {
        data.error = i;
        libs::array<uint8_t, 1> read;
        i2c.writeRead(0x1A, {}, read);
        TEST_ASSERT_EQUAL_INT(i, read[0]);
    }
}

void test_read_one_byte() {
    uint8_t offset = 1;

    for (uint8_t times = 0; times < 5; ++times) {
        data.error = offset;
        for (uint8_t i = 0; i < data.data.size(); ++i) {
            data.data[i] = offset+i;
        }

        for (uint8_t i = 0; i < data.data.size()-1; ++i) {
            libs::array<uint8_t, 1> write = {i};
            libs::array<uint8_t, 2> read;
            i2c.writeRead(0x1A, write, read);

            TEST_ASSERT_EQUAL_INT(data.error, read[0]);
            TEST_ASSERT_EQUAL_INT(offset++, read[1]);
        }
    }
}

void test_read_five_bytes() {
    uint8_t offset = 0;

    for (uint8_t times = 0; times < 5; ++times) {
        data.error = offset;
        for (uint8_t i = 0; i < data.data.size(); ++i) {
            data.data[i] = offset+153*i;
        }

        for (uint8_t i = 0; i < data.data.size()-5; ++i) {
            libs::array<uint8_t, 1> write = {i};
            libs::array<uint8_t, 6> read;
            i2c.writeRead(0x1A, write, read);

            TEST_ASSERT_EQUAL_INT(data.error, read[0]);
            compare_spans(libs::make_span(data.data).subspan(i, 5),
                          libs::make_span(read).subspan(1, 5));
        }

        offset += 73;
    }
}

void test_read_full() {
    uint8_t offset = 0;

    for (uint8_t times = 0; times < 5; ++times) {
        data.error = offset-1;
        for (uint8_t i = 0; i < data.data.size(); ++i) {
            data.data[i] = offset + 153 * i;
        }
        libs::array<uint8_t, 1> write = {0};
        libs::array<uint8_t, data.data.size()+1> read;
        i2c.writeRead(0x1A, write, read);

        TEST_ASSERT_EQUAL_INT(data.error, read[0]);
        compare_spans(data.data, libs::make_span(read).subspan(1, data.data.size()));

        offset += 52;
    }
}

void test_read_overflow() {
    data.error = 0xFF;
    for (uint8_t i = 0; i < data.data.size(); ++i) {
        data.data[i] = i;
    }

    libs::array<uint8_t, 1> write = {80};
    libs::array<uint8_t, 50> read;
    i2c.writeRead(0x1A, write, read);

    TEST_ASSERT_EQUAL_INT(data.error, read[0]);

    for(size_t i = 80; i < data.data.size(); ++i) {
        TEST_ASSERT_EQUAL_INT(i, read[i-80+1]);
    }
    for(size_t i = data.data.size(); i < read.size(); ++i) {
        TEST_ASSERT_EQUAL_INT(0, read[i-80+1]);
    }
}

// ----------------  main  -------------------

void TEST_ObcInterface() {
    UNITY_BEGIN();

    obc.set_memory(&data);
    obc.init();
    sei();

    i2c.init();
    
    RUN_TEST(test_cmd_empty);
    RUN_TEST(test_cmd_short);
    RUN_TEST(test_cmd_longer);
    RUN_TEST(test_cmd_full);
    RUN_TEST(test_cmd_overflow);
    RUN_TEST(test_cmd_large_overflow);

    RUN_TEST(test_read_only_error);
    RUN_TEST(test_read_one_byte);
    RUN_TEST(test_read_five_bytes);
    RUN_TEST(test_read_full);
    RUN_TEST(test_read_overflow);

    cli();

    UNITY_END();
}
