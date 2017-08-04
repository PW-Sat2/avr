#include <unity.h>
#include <hal/hal>

#include "LclInterface.h"

using namespace eps;

template<int tag>
struct WritePinIOMock {
    static bool value;
    static void set() {
        value = true;
    }
    static void reset() {
        value = false;
    }
};
template<int tag>
bool WritePinIOMock<tag>::value;

template<int tag>
struct ReadPinIOMock {
    static bool value;
};

using OnMock    = WritePinIOMock<0>;
using FlagBMock = ReadPinIOMock<0>;

using LCL1 = LclInterface<OnMock, FlagBMock, int, 0, 't', 'e', 's', 't'>;
using LCL2 =
    LclInterface<OnMock, FlagBMock, uint32_t, 0x7A4785, 'a', 'b', 'c', 'd', 'e', 'f'>;

void test_LclInterface_name() {
    TEST_ASSERT_EQUAL_STRING("test", LCL1::name);
    TEST_ASSERT_EQUAL_STRING("abcdef", LCL2::name);
}

void test_LclInterface_id() {
    static_assert(std::is_same<int, LCL1::IdType>::value, "");
    static_assert(std::is_same<std::uint32_t, LCL2::IdType>::value, "");
    TEST_ASSERT_EQUAL_INT(0, LCL1::id);
    TEST_ASSERT_EQUAL_INT(0x7A4785, LCL2::id);
}

void test_LclInterface_on() {
    OnMock::value = true;
    LCL1::on();
    TEST_ASSERT_FALSE(OnMock::value);
}

void test_LclInterface_off() {
    OnMock::value = false;
    LCL1::off();
    TEST_ASSERT_TRUE(OnMock::value);
}

void test_LclInterface() {
    UnityBegin("");

    RUN_TEST(test_LclInterface_name);
    RUN_TEST(test_LclInterface_id);
    RUN_TEST(test_LclInterface_on);
    RUN_TEST(test_LclInterface_off);

    UnityEnd();
}
