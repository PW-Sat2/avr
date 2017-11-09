#include "MpptUpdater.h"
#include <math.h>
#include <unity.h>
#include "MainTimer.h"

using namespace eps;

static_assert(voltage_to_adc_reading(0) == 0, "");
static_assert(voltage_to_adc_reading(9.405) == 2047, "");
static_assert(voltage_to_adc_reading(18.81) == 4095, "");

using Cfg = eps::MpptSettings::Y;

static constexpr auto dac_upper_boundary = Cfg::dac_upper_boundary;
static constexpr auto dac_lower_boundary = Cfg::dac_lower_boundary;
static constexpr uint12_t max_mpp_error  = 10;

template<typename T, typename Tm>
uint16_t mock_only_mppt_tracking(T& updater, Tm tm) {
    updater.update_tm(tm);
    updater.mpp_tracking();
    updater.dac_constrain();
    return updater.dac;
}

template<typename T>
uint16_t mock_only_sweep_down(T& updater) {
    updater.dac_sweep_down();
    updater.dac_constrain();
    return updater.dac;
}

template<typename T>
uint16_t mock_only_sweep_up(T& updater) {
    updater.dac_sweep_up();
    updater.dac_constrain();
    return updater.dac;
}

void check_constrains(uint16_t output) {
    TEST_ASSERT_TRUE(dac_lower_boundary <= output);
    TEST_ASSERT_TRUE(output <= dac_upper_boundary);
}

void test_MpptUpdater_Init() {
    MpptUpdater<Cfg> updater;
    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    auto output = mock_only_mppt_tracking(updater, tm);

    TEST_ASSERT_EQUAL(dac_upper_boundary, output);
}

void test_MpptUpdater_UpperBoundary() {
    MpptUpdater<Cfg> updater(dac_lower_boundary);

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;
    mock_only_mppt_tracking(updater, tm);
    tm.solar_voltage = 100;

    for (int i = 0; i < 500; i++) {
        updater.update_tm(tm);
        auto output = mock_only_mppt_tracking(updater, tm);

        check_constrains(output);
    }

    updater.update_tm(tm);
    auto output = mock_only_mppt_tracking(updater, tm);
    TEST_ASSERT_EQUAL(dac_upper_boundary, output);
}

void test_MpptUpdater_LowerBoundary() {
    MpptUpdater<Cfg> updater(dac_upper_boundary);

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;
    updater.update_tm(tm);
    mock_only_mppt_tracking(updater, tm);
    tm.solar_voltage = 0;

    for (int i = 0; i < 500; i++) {
        updater.update_tm(tm);
        auto output = mock_only_mppt_tracking(updater, tm);

        check_constrains(output);
    }

    updater.update_tm(tm);
    auto output = mock_only_mppt_tracking(updater, tm);
    TEST_ASSERT_EQUAL(dac_lower_boundary, output);
}

/*!
 * Function to clamp a value to given limits.
 * @param value - value to clamp.
 * @param lower_limit - value for lower limit.
 * @param upper_limit - value for upper limit.
 * @return clamped value with lower and upper limits.
 */
constexpr uint16_t
clamp_value(float value, uint16_t lower_limit, uint16_t upper_limit) {
    if (value > static_cast<float>(upper_limit)) {
        return upper_limit;
    }
    if (value < static_cast<float>(lower_limit)) {
        return lower_limit;
    }
    return value;
}

/*!
 * Function to simulate sinus-like Voltage-vs-Power curve.
 * @param solar_voltage 12-bit solar voltage.
 * @return Calculated solar power at specified solar voltage.
 */
constexpr uint16_t mock_PVCurveSinus(uint16_t solar_voltage) {
    float sine = sin(((solar_voltage - 1300) / 4096.0) * M_PI * 1.5) * 1100 + 1000;
    uint16_t output_power = clamp_value(sine, 0, 4095);
    return output_power;
}

/*!
 * Function to find MPP on a Voltage-vs-Power curve.
 * @return Power value at MPP.
 */
constexpr uint16_t mock_PVCurve_FindMppPower() {
    uint16_t mpp_power = 0;

    for (uint16_t solar_voltage = 0; solar_voltage < 4095; solar_voltage += 8) {
        uint16_t solar_power = mock_PVCurveSinus(solar_voltage);

        if (solar_power > mpp_power) {
            mpp_power = solar_power;
        }
    }

    return mpp_power;
}

void test_MpptUpdater_MppTracking_SinusPVCurve_StartingPointSweep() {
    uint12_t dac_set             = 0;
    constexpr uint12_t mpp_power = mock_PVCurve_FindMppPower();

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 4000;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    for (uint16_t starting_point = dac_lower_boundary;
         starting_point <= dac_upper_boundary;
         starting_point += 8) {
        MpptUpdater<Cfg> updater(starting_point);

        for (int i = 0; i < 1000; i++) {
            updater.update_tm(tm);
            dac_set = mock_only_mppt_tracking(updater, tm);

            tm.solar_voltage = mock_PVCurveSinus(dac_set) + ((rand() % 3) - 1);

            check_constrains(dac_set);
        }


        TEST_ASSERT_LESS_THAN(mpp_power + max_mpp_error, tm.solar_voltage);
        TEST_ASSERT_GREATER_THAN(mpp_power - max_mpp_error, tm.solar_voltage);
    }

    LOG_DEBUG("MppTracking mode found MPP at: %u", tm.solar_voltage);
}

void test_MpptUpdater_MppTracking_SinusPVCurve_SolarCurrentSweep() {
    constexpr uint12_t mpp_power = mock_PVCurve_FindMppPower();

    Telemetry::SingleMpptChannel tm;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    for (uint16_t solar_current = 1; solar_current <= 4095; solar_current += 16) {
        MpptUpdater<Cfg> updater(3000);
        uint12_t dac_set = 0;
        tm.solar_current = solar_current;

        for (int i = 0; i < 1000; i++) {
            updater.update_tm(tm);
            dac_set = mock_only_mppt_tracking(updater, tm);

            tm.solar_voltage = mock_PVCurveSinus(dac_set) + ((rand() % 3) - 1);
            check_constrains(dac_set);
        }

        TEST_ASSERT_LESS_THAN(mpp_power + max_mpp_error, tm.solar_voltage);
        TEST_ASSERT_GREATER_THAN(mpp_power - max_mpp_error, tm.solar_voltage);
    }

    LOG_DEBUG("MppTracking mode found MPP at: %u", tm.solar_voltage);
}

void test_MpptUpdater_DacSweepDown() {
    MpptUpdater<Cfg> updater(4095);

    updater.dac_constrain();
    uint16_t last = updater.dac;
    check_constrains(last);

    TEST_ASSERT_EQUAL(dac_upper_boundary, last);

    for (int i = 0; i < 300; i++) {
        auto output = mock_only_sweep_down(updater);
        check_constrains(output);
        TEST_ASSERT_TRUE(output < last || output == dac_lower_boundary);
        last = output;
    }

    TEST_ASSERT_EQUAL(dac_lower_boundary, last);
}

void test_MpptUpdater_DacSweepUp() {
    MpptUpdater<Cfg> updater(1);

    uint16_t last = mock_only_sweep_up(updater);
    TEST_ASSERT_EQUAL(dac_lower_boundary, last);

    for (int i = 0; i < 300; i++) {
        auto output = mock_only_sweep_up(updater);
        check_constrains(output);
        TEST_ASSERT_TRUE(output > last || output == dac_upper_boundary);
        last = output;
    }

    TEST_ASSERT_EQUAL(dac_upper_boundary, last);
}

void test_MpptUpdater_MppRestart() {
    MpptUpdater<Cfg> updater(127);

    updater.mpp_restart();
    updater.dac_constrain();
    TEST_ASSERT_EQUAL(dac_upper_boundary, updater.dac);
}

void test_MpptUpdater_CalculatePower() {
    MpptUpdater<Cfg> updater(dac_lower_boundary);
    Telemetry::SingleMpptChannel tm;

    auto test = [&](uint16_t a, uint16_t b, uint32_t result) {
        tm.solar_voltage = a;
        tm.solar_current = b;

        updater.tick(tm);

        TEST_ASSERT_EQUAL_UINT32(result, updater.calculate_power());
    };

    test(0, 0xFFFF, 0);
    test(0xFFFF, 0, 0);

    test(0xFFFF, 15789, 1034732115ULL);
    test(0xFFFF, 0xFFFF, 0xfffe0001ULL);
}


void test_MpptUpdater_load_detect() {
    auto check =
        [](auto start, auto solar, auto out, bool overloaded, bool underloaded) {
            MpptUpdater<Cfg> updater(start);

            Telemetry::SingleMpptChannel tm;
            tm.solar_voltage  = solar;
            tm.output_voltage = out;
            tm.solar_current  = 0;

            auto dac = updater.tick(tm);

            if (overloaded) {
                TEST_ASSERT_EQUAL(dac_upper_boundary, dac);
            } else {
                TEST_ASSERT_LESS_THAN(dac_upper_boundary, dac);
            }

            auto expect = start - Cfg::dac_sweep_down_value;
            if (underloaded) {
                TEST_ASSERT_EQUAL(expect, dac);
            } else {
                TEST_ASSERT_GREATER_THAN(expect, dac);
            }

            check_constrains(dac);
        };

    auto lower = Cfg::dac_lower_boundary;
    auto solar = Cfg::min_solar_voltage;
    auto out   = Cfg::min_output_voltage;

    check(lower + 1, solar, out + 10, false, false);
    check(lower + 1, solar - 10, out + 10, true, false);
    check(lower, solar, out + 10, true, false);
    check(lower, 4095, 0, true, false);

    check(lower + 1000, solar, out, false, false);
    check(lower + 1000, solar + 1, out - 1, false, true);
    check(lower + 1000, solar + 10, out + 10, false, false);
}

void test_MpptUpdater_delta_power() {
    MpptUpdater<Cfg> updater(2500);

    updater.dac = Cfg::dac_lower_boundary + 10;

    Telemetry::SingleMpptChannel tm;
    tm.solar_voltage  = Cfg::min_solar_voltage;
    tm.output_voltage = 0;

    tm.solar_current = 0;
    updater.tick(tm);

    tm.solar_current = 20000;
    updater.dac      = 2000;

    for (int i = 1; i <= 10; ++i) {
        updater.tick(tm);
        TEST_ASSERT_EQUAL(2000 + Cfg::dac_sweep_up_value - i * Cfg::dac_perturb,
                          updater.dac);
    }

    tm.solar_current = 0;
    updater.tick(tm);
    TEST_ASSERT_EQUAL(
        2000 + 2 * Cfg::dac_sweep_up_value - 11 * Cfg::dac_perturb, updater.dac);
}

void test_MpptUpdater_tick() {
    constexpr auto mpp_power = mock_PVCurve_FindMppPower();

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 4000;
    tm.solar_voltage  = 10;
    tm.output_voltage = 5000;

    for (uint16_t starting_point = dac_lower_boundary + 100;
         starting_point <= dac_upper_boundary;
         starting_point += 80) {
        MpptUpdater<Cfg> updater(starting_point);

        uint12_t dac_set = starting_point;

        for (int i = 0; i < 1000; i++) {
            tm.solar_voltage =
                mock_PVCurveSinus(dac_set) + 1000 + ((rand() % 3) - 1);

            if (dac_set < 2500) {
                tm.solar_current = 0;
            } else {
                tm.solar_current = 4000;
            }

            tm.output_voltage = tm.solar_voltage / 2;

            updater.update_tm(tm);
            dac_set = updater.tick(tm);

            check_constrains(dac_set);
        }

        TEST_ASSERT_LESS_THAN(mpp_power + max_mpp_error, tm.solar_voltage - 1000);
        TEST_ASSERT_GREATER_THAN(mpp_power - max_mpp_error,
                                 tm.solar_voltage - 1000);

        LOG_DEBUG("MppTracking mode found MPP at: %u", tm.solar_voltage);
    }
}

/*!
 * Function to extract MPPT mode from a STATE byte.
 * @return Extracted MPPT mode.
 */
uint8_t extract_mppt_mode(uint8_t mppt_state) {
    return mppt_state >> 1;
}

/*!
 * Function to extract MPP reached flag from a STATE byte.
 * @return Extracted MPP reached flag.
 */
uint8_t extract_mpp_reached_flag(uint8_t mppt_state) {
    return mppt_state && 0x01;
}

void test_MpptUpdater_get_state() {
    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 100;
    tm.solar_voltage  = 3000;
    tm.output_voltage = 10;

    MpptUpdater<Cfg> updater(1);

    TEST_ASSERT_EQUAL(MpptMode::Unknown, extract_mppt_mode(updater.get_state()));

    for (int i = 0; i < 10; i++) {
        updater.tick(tm);
    }

    TEST_ASSERT_EQUAL(MpptMode::SolarPanelIsUnderloaded,
                      extract_mppt_mode(updater.get_state()));

    tm.solar_current = 10;
    tm.solar_voltage = 10;
    updater.tick(tm);

    TEST_ASSERT_EQUAL(MpptMode::SolarPanelIsOverloaded,
                      extract_mppt_mode(updater.get_state()));

    tm.solar_current  = 1000;
    tm.output_voltage = 3000;
    uint12_t dac_set  = 2000;

    TEST_ASSERT_EQUAL(MppTrackingState::MppNotReachedYet,
                      extract_mpp_reached_flag(updater.get_state()));

    for (int i = 0; i < 100; i++) {
        tm.solar_voltage = mock_PVCurveSinus(dac_set) + 1000 + ((rand() % 3) - 1);
        dac_set          = updater.tick(tm);

        TEST_ASSERT_EQUAL(MpptMode::MppTracking,
                          extract_mppt_mode(updater.get_state()));
    }

    TEST_ASSERT_EQUAL(MppTrackingState::MppNotReachedYet,
                      extract_mpp_reached_flag(updater.get_state()));

    for (int i = 0; i < 1500; i++) {
        tm.solar_voltage = mock_PVCurveSinus(dac_set) + 1000 + ((rand() % 3) - 1);
        dac_set          = updater.tick(tm);

        TEST_ASSERT_EQUAL(MpptMode::MppTracking,
                          extract_mppt_mode(updater.get_state()));
    }

    TEST_ASSERT_EQUAL(MppTrackingState::MppReached,
                      extract_mpp_reached_flag(updater.get_state()));
}

void test_MpptUpdater() {
    UnityBegin("");

    RUN_TEST(test_MpptUpdater_Init);
    RUN_TEST(test_MpptUpdater_UpperBoundary);
    RUN_TEST(test_MpptUpdater_LowerBoundary);
    RUN_TEST(test_MpptUpdater_MppTracking_SinusPVCurve_StartingPointSweep);
    RUN_TEST(test_MpptUpdater_MppTracking_SinusPVCurve_SolarCurrentSweep);
    RUN_TEST(test_MpptUpdater_DacSweepDown);
    RUN_TEST(test_MpptUpdater_DacSweepUp);
    RUN_TEST(test_MpptUpdater_MppRestart);
    RUN_TEST(test_MpptUpdater_CalculatePower);
    RUN_TEST(test_MpptUpdater_load_detect);
    RUN_TEST(test_MpptUpdater_delta_power);
    RUN_TEST(test_MpptUpdater_tick);
    RUN_TEST(test_MpptUpdater_get_state);
    UnityEnd();
}
