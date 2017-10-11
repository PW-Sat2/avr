#include <unity.h>
#include <hal/hal>

#include <avr/pgmspace.h>

#include "TemperatureSensorsValidation.h"


void test_Temperature_LMT87_always_valid() {
    for (uint16_t i = 0; i < 0xFFFF; ++i) {
        TEST_ASSERT_FLOAT_IS_DETERMINATE(lmt87(i));
        TEST_ASSERT_FLOAT_IS_DETERMINATE(lmt87_adc124(i << 2));
    }
    TEST_ASSERT_FLOAT_IS_DETERMINATE(lmt87(0xFFFF));
    TEST_ASSERT_FLOAT_IS_DETERMINATE(lmt87_adc124(0xFFFF << 2));
}

void test_Temperature_LMT87() {
    TEST_ASSERT_FLOAT_WITHIN(5, 150., lmt87(183));
    TEST_ASSERT_FLOAT_WITHIN(2, 100., lmt87(429));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 50., lmt87(668));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20., lmt87(807));
    TEST_ASSERT_FLOAT_WITHIN(0.4, 0., lmt87(898));
    TEST_ASSERT_FLOAT_WITHIN(1, -20., lmt87(989));
    TEST_ASSERT_FLOAT_WITHIN(5, -50., lmt87(1117));

    TEST_ASSERT_FLOAT_WITHIN(5, 150., lmt87_adc124(666));
    TEST_ASSERT_FLOAT_WITHIN(2, 100., lmt87_adc124(1561));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 50., lmt87_adc124(2431));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20., lmt87_adc124(2937));
    TEST_ASSERT_FLOAT_WITHIN(0.4, 0., lmt87_adc124(3268));
    TEST_ASSERT_FLOAT_WITHIN(1, -20., lmt87_adc124(3599));
    TEST_ASSERT_FLOAT_WITHIN(5, -50., lmt87_adc124(4065));
}

void test_Temperature_TMP121_always_valid() {
    for (uint16_t i = 0; i < 0xFFFF; ++i) {
        TEST_ASSERT_FLOAT_IS_DETERMINATE(tmp121(i));
    }
    TEST_ASSERT_FLOAT_IS_DETERMINATE(tmp121(0xFFFF));
}

void test_Temperature_TMP121() {
    TEST_ASSERT_EQUAL_FLOAT(150.0, tmp121(0x960));
    TEST_ASSERT_EQUAL_FLOAT(125.0, tmp121(0x7D0));
    TEST_ASSERT_EQUAL_FLOAT(25.0, tmp121(0x0190));
    TEST_ASSERT_EQUAL_FLOAT(0.0625, tmp121(0x1));
    TEST_ASSERT_EQUAL_FLOAT(0, tmp121(0x0));
    TEST_ASSERT_EQUAL_FLOAT(-0.0625, tmp121(0x1FFF));
    TEST_ASSERT_EQUAL_FLOAT(-25, tmp121(0x1E70));
    TEST_ASSERT_EQUAL_FLOAT(-55, tmp121(0x1C90));
}

void test_Temperature_PT1000_valid() {
    float prev = pt1000(0);
    TEST_ASSERT_FLOAT_IS_DETERMINATE(prev);

    for (uint16_t i = 1; i < 713; ++i) {
        float now = pt1000(i);
        TEST_ASSERT_FLOAT_IS_DETERMINATE(now);
        TEST_ASSERT_TRUE(now > prev);
        prev = now;
    }

    for (uint16_t i = 713; i < 1024; ++i) {
        TEST_ASSERT_FLOAT_IS_NOT_DETERMINATE(pt1000(i));
    }

    for (uint16_t i = 1024; i < 0xFFFF - 10; i += 10) {
        float now = pt1000(i);
        TEST_ASSERT_FLOAT_IS_DETERMINATE(now);
        TEST_ASSERT_LESS_THAN(-500, now);
    }
}

void test_Temperature_PT1000() {
    TEST_ASSERT_FLOAT_WITHIN(3, -200, pt1000(54));
    TEST_ASSERT_FLOAT_WITHIN(1, -150, pt1000(110));
    TEST_ASSERT_FLOAT_WITHIN(0.5, -100, pt1000(158));
    TEST_ASSERT_FLOAT_WITHIN(0.5, -50, pt1000(200));
    TEST_ASSERT_FLOAT_WITHIN(0.5, 0, pt1000(238));
    TEST_ASSERT_FLOAT_WITHIN(0.5, 50, pt1000(272));
    TEST_ASSERT_FLOAT_WITHIN(1, 100, pt1000(302));
    TEST_ASSERT_FLOAT_WITHIN(1, 150, pt1000(330));
    TEST_ASSERT_FLOAT_WITHIN(1, 200, pt1000(356));
}

float offset = -30000;
float dummy_conversion(uint16_t raw) {
    return (raw + offset) / 100.0;
}

uint16_t value;

static constexpr char name[] PROGMEM = "x";
TemperatureConversion conversion(dummy_conversion, -70.5, 89.3, name, value);
void test_Temperature_Conversion() {
    auto checkOK = [](uint16_t raw, float temperature) {
        value  = raw;
        auto x = conversion.get();
        TEST_ASSERT_TRUE(x);
        TEST_ASSERT_EQUAL_FLOAT(temperature, *x);
    };

    auto checkFail = [](uint16_t raw) {
        value  = raw;
        auto x = conversion.get();
        TEST_ASSERT_FALSE(x);
    };

    for (uint16_t i = 0; i < 22950; i += 100) {
        checkFail(i);
    }
    checkFail(22950);

    for (uint16_t i = 22951; i < 38951; i += 100) {
        checkOK(i, (static_cast<float>(i) - 30000.) / 100.0);
    }

    checkOK(38929, (38929. - 30000.) / 100.0);
    for (uint16_t i = 38930; i < 0xFFFF - 100; i += 100) {
        checkFail(i);
    }
    checkFail(0xFFFF);
}

void test_Temperature_Conversion_NaN() {
    offset = std::numeric_limits<float>::quiet_NaN();
    TEST_ASSERT_FALSE(conversion.get());

    offset = 0;
    value  = 0;
    TEST_ASSERT_TRUE(conversion.get());
}

void test_Temperature_merge_max() {
    TEST_ASSERT_EQUAL_FLOAT(15, merge_temperatures({10.0, 15.0}));
    TEST_ASSERT_EQUAL_FLOAT(115.0, merge_temperatures({115.0}));
    TEST_ASSERT_EQUAL_FLOAT(15.5, merge_temperatures({10.0, 15.5, 2., 3., 7.}));
}

void test_Temperature_merge_drop_incorrect() {
    std::experimental::optional<float> f = 8000;

    f = {};

    TEST_ASSERT_EQUAL_FLOAT(15, merge_temperatures({{}, 10.0, 15.0, f, {}}));
    TEST_ASSERT_EQUAL_FLOAT(115.0, merge_temperatures({f, 115.0, {}}));
    TEST_ASSERT_EQUAL_FLOAT(
        15.5, merge_temperatures({10.0, f, 15.5, {}, 2., f, f, 3., 7.}));
}

void test_Temperature_merge_none_valid() {
    std::experimental::optional<float> f = 8000;

    f = {};

    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({}));
    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({{}}));
    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({{}, {}}));
    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({{}, {}, f}));
    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({{}, {}, f, {}}));
    TEST_ASSERT_EQUAL_FLOAT(10, merge_temperatures({{}, {}, f, f, f, f}));
}

void test_TemperatureCheck() {
    UnityBegin("");

    RUN_TEST(test_Temperature_LMT87_always_valid);
    RUN_TEST(test_Temperature_LMT87);

    RUN_TEST(test_Temperature_TMP121_always_valid);
    RUN_TEST(test_Temperature_TMP121);

    RUN_TEST(test_Temperature_PT1000_valid);
    RUN_TEST(test_Temperature_PT1000);

    RUN_TEST(test_Temperature_Conversion);
    RUN_TEST(test_Temperature_Conversion_NaN);

    RUN_TEST(test_Temperature_merge_max);
    RUN_TEST(test_Temperature_merge_drop_incorrect);
    RUN_TEST(test_Temperature_merge_none_valid);

    UnityEnd();
}
