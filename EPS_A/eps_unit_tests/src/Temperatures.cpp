#include <unity.h>

#include "Temperatures.h"

using namespace eps;
using namespace avr;
using namespace avr::temperatures;

static auto tm_general = (Telemetry::General*)(&Eps::telemetry.general.get());
static auto tm_mppt = (Telemetry::AllMpptChannels*)(&Eps::telemetry.mppt.get());

template<typename T>
void test_lmt87(uint16_t& reg, const T& object) {
    reg = 807;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 20., *object());

    reg = 898;
    TEST_ASSERT_FLOAT_WITHIN(0.4, 0., *object());
}

template<typename T>
void test_lmt87_12b(uint16_t& reg, const T& object) {
    reg = 2431;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 50., *object());

    reg = 2937;
    TEST_ASSERT_FLOAT_WITHIN(0.4, 20., *object());
}

template<typename T>
void test_tmp121(uint16_t& reg, const T& object) {
    reg = 0x0190;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 25., *object());

    reg = 1;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0625, *object());
}

void test_Temperatures_ok() {
    test_lmt87(tm_general->controller_a.temperature, ctrla);
    test_lmt87(tm_general->controller_a.supply_temperature, supply);
    test_lmt87(tm_general->battery_controller.temperature, batc);
    test_lmt87(tm_general->dcdc_5v_temperature, dcdc5v);
    test_lmt87(tm_general->dcdc_3v3_temperature, dcdc3v3);

    test_tmp121(tm_general->battery_pack.temperature_a, battery_pack_a);
    test_tmp121(tm_general->battery_pack.temperature_b, battery_pack_b);

    test_lmt87_12b(tm_mppt->mpptx.temperature, mppt_x);
    test_lmt87_12b(tm_mppt->mpptyn.temperature, mppt_yn);
    test_lmt87_12b(tm_mppt->mpptyp.temperature, mppt_yp);
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
    test_limits(tm_general->controller_a.temperature, ctrla, eps_min, eps_max);
    test_limits(
        tm_general->controller_a.supply_temperature, supply, eps_min, eps_max);
    test_limits(tm_general->battery_controller.temperature, batc, eps_min, eps_max);
    test_limits(tm_general->dcdc_5v_temperature, dcdc5v, eps_min, eps_max);
    test_limits(tm_general->dcdc_3v3_temperature, dcdc3v3, eps_min, eps_max);

    test_limits(
        tm_general->battery_pack.temperature_a, battery_pack_a, bp_min, bp_max);
    test_limits(
        tm_general->battery_pack.temperature_b, battery_pack_b, bp_min, bp_max);

    test_limits(tm_mppt->mpptx.temperature, mppt_x, eps_min, eps_max);
    test_limits(tm_mppt->mpptyn.temperature, mppt_yn, eps_min, eps_max);
    test_limits(tm_mppt->mpptyp.temperature, mppt_yp, eps_min, eps_max);
}
void test_Temperatures() {
    UnityBegin("");

    RUN_TEST(test_Temperatures_ok);
    RUN_TEST(test_Temperatures_over_limit);

    UnityEnd();
}
