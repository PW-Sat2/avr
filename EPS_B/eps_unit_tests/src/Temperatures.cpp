#include <unity.h>

#include "Temperatures.h"

using namespace eps;
using namespace avr;
using namespace avr::temperatures;

static auto tm_general = (Telemetry::General*)(&Eps::telemetry.general.get());

template<typename T>
void test_lmt87(uint16_t& reg, const T& object) {
    reg = 807;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20., *object());

    reg = 898;
    TEST_ASSERT_FLOAT_WITHIN(0.4, 0., *object());
}

template<typename T>
void test_pt1000(uint16_t& reg, const T& object) {
    reg = 238;
    TEST_ASSERT_FLOAT_WITHIN(0.5, 0., *object());

    reg = 272;
    TEST_ASSERT_FLOAT_WITHIN(0.5, 50, *object());
}

void test_Temperatures_ok() {
    test_lmt87(tm_general->controller_b.temperature, ctrlb);
    test_lmt87(tm_general->controller_b.supply_temperature, supply);

    test_pt1000(tm_general->battery_pack.temperature, battery_pack);
}


int putxchar(char, FILE*) {
    return 1;
}

template<typename T>
void test_limits(uint16_t& reg, const T& object, float min, float max) {
    for (uint16_t i = 0; i < 0xFFFF - 10; i += 10) {
        reg = i;

        stdout->put = putxchar;
        auto now    = object();

        hal::Serial0.redirect_stdio();

        if (now) {
            TEST_ASSERT_GREATER_THAN(min, *now);
            TEST_ASSERT_LESS_THAN(max, *now);
        }
    }
}

constexpr static float eps_min = -80, eps_max = 100;
constexpr static float bp_min = -80, bp_max = 100;

void test_Temperatures_over_limit() {
    test_limits(tm_general->controller_b.temperature, ctrlb, eps_min, eps_max);
    test_limits(
        tm_general->controller_b.supply_temperature, supply, eps_min, eps_max);

    test_limits(tm_general->battery_pack.temperature, battery_pack, bp_min, bp_max);
}

void test_Temperatures() {
    UnityBegin("");

    RUN_TEST(test_Temperatures_ok);
    RUN_TEST(test_Temperatures_over_limit);

    UnityEnd();
}
