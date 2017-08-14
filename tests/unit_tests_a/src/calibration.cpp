#include <unity.h>
#include <hal/hal>

#include "calibration/TMP121.h"

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


void test_calibration() {
    UnityBegin("");

    RUN_TEST(test_calibration_TMP121);

    UnityEnd();
}
