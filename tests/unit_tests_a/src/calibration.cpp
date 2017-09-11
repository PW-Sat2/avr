#include <unity.h>
#include <hal/hal>

#include "calibration/Adc.h"
#include "calibration/LMT87.h"
#include "calibration/TMP121.h"
#include "calibration/VoltageDivider.h"

using namespace avr::calibration;

void test_calibration_TMP121() {
    TEST_ASSERT_EQUAL_FLOAT(150.0, tmp121::raw_to_celsius(0x960));
    TEST_ASSERT_EQUAL_FLOAT(125.0, tmp121::raw_to_celsius(0x7D0));
    TEST_ASSERT_EQUAL_FLOAT(25.0, tmp121::raw_to_celsius(0x0190));
    TEST_ASSERT_EQUAL_FLOAT(0.0625, tmp121::raw_to_celsius(0x1));
    TEST_ASSERT_EQUAL_FLOAT(0, tmp121::raw_to_celsius(0x0));
    TEST_ASSERT_EQUAL_FLOAT(-0.0625, tmp121::raw_to_celsius(0x1FFF));
    TEST_ASSERT_EQUAL_FLOAT(-25, tmp121::raw_to_celsius(0x1E70));
    TEST_ASSERT_EQUAL_FLOAT(-55, tmp121::raw_to_celsius(0x1C90));
}

void test_calibration_LMT87() {
    TEST_ASSERT_EQUAL_FLOAT(194.247, lmt87::volts_to_celsius(0));
    TEST_ASSERT_EQUAL_FLOAT(120.62, lmt87::volts_to_celsius(1));
    TEST_ASSERT_EQUAL_FLOAT(83.8065, lmt87::volts_to_celsius(1.5));
    TEST_ASSERT_EQUAL_FLOAT(46.9931, lmt87::volts_to_celsius(2));
    TEST_ASSERT_EQUAL_FLOAT(10.17984, lmt87::volts_to_celsius(2.5));
    TEST_ASSERT_EQUAL_FLOAT(-26.6338, lmt87::volts_to_celsius(3));
    TEST_ASSERT_EQUAL_FLOAT(-63.4472, lmt87::volts_to_celsius(3.5));
}

void test_calibration_Adc() {
    using Adc1 = Adc<10, 5000>;
    TEST_ASSERT_EQUAL_FLOAT(0.0, Adc1::reading_to_voltage(0));
    TEST_ASSERT_EQUAL_FLOAT(1.236559, Adc1::reading_to_voltage(253));
    TEST_ASSERT_EQUAL_FLOAT(5.0, Adc1::reading_to_voltage(1023));

    using Adc2 = Adc<16, 2048>;
    TEST_ASSERT_EQUAL_FLOAT(0.0, Adc2::reading_to_voltage(0));
    TEST_ASSERT_EQUAL_FLOAT(1.02398, Adc2::reading_to_voltage(32767));
    TEST_ASSERT_EQUAL_FLOAT(2.0, Adc2::reading_to_voltage(63999));
    TEST_ASSERT_EQUAL_FLOAT(2.048, Adc2::reading_to_voltage(65535));
}

void test_calibration_voltage_divider() {
    constexpr static VoltageDivider div2(10.0f, 10.0f);

    TEST_ASSERT_EQUAL_FLOAT(1.0f, div2.lower_to_full(0.5f));
    TEST_ASSERT_EQUAL_FLOAT(2.0f, div2.lower_to_full(1.0f));

    constexpr static VoltageDivider div7p234(20.0f, 20 * 6.234f);

    TEST_ASSERT_EQUAL_FLOAT(0.5f * 7.234, div7p234.lower_to_full(0.5f));
    TEST_ASSERT_EQUAL_FLOAT(7.234f, div7p234.lower_to_full(1.0f));

    constexpr static VoltageDivider div_large(150.0e3f, 470.0e3f);

    TEST_ASSERT_EQUAL_FLOAT(4.133333333, div_large.lower_to_full(1.0f));
    TEST_ASSERT_EQUAL_FLOAT(13.64, div_large.lower_to_full(3.3f));
    TEST_ASSERT_EQUAL_FLOAT(18.6, div_large.lower_to_full(4.5f));
}

void test_calibration() {
    UnityBegin("");

    RUN_TEST(test_calibration_TMP121);
    RUN_TEST(test_calibration_LMT87);
    RUN_TEST(test_calibration_Adc);
    RUN_TEST(test_calibration_voltage_divider);

    UnityEnd();
}
