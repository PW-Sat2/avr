#include "MpptUpdater.h"
#include <math.h>
#include <unity.h>
#include "MainTimer.h"

using namespace eps;

static constexpr uint12_t dac_upper_boundary = 4080;
static constexpr uint12_t dac_lower_boundary = 1500;
static constexpr uint12_t max_mpp_error      = 10;

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

void test_MpptUpdater_Init() {
    MpptUpdater<eps::MpptSettings::Y> updater;
    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    auto output = mock_only_mppt_tracking(updater, tm);

    TEST_ASSERT_EQUAL(dac_upper_boundary, output);
}

void test_MpptUpdater_UpperBoundary() {
    MpptUpdater<eps::MpptSettings::Y> updater;

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;
    mock_only_mppt_tracking(updater, tm);
    tm.solar_voltage = 100;

    for (int i = 0; i < 150; i++) {
        updater.update_tm(tm);
        auto output = mock_only_mppt_tracking(updater, tm);

        TEST_ASSERT_LESS_THAN(dac_upper_boundary + 1, output);
        TEST_ASSERT_GREATER_THAN(dac_lower_boundary - 1, output);
    }

    updater.update_tm(tm);
    auto output = mock_only_mppt_tracking(updater, tm);
    TEST_ASSERT_EQUAL(dac_upper_boundary, output);
}

void test_MpptUpdater_LowerBoundary() {
    MpptUpdater<eps::MpptSettings::Y> updater;

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 10;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;
    updater.update_tm(tm);
    mock_only_mppt_tracking(updater, tm);
    tm.solar_voltage = 0;

    for (int i = 0; i < 1500; i++) {
        updater.update_tm(tm);
        auto output = mock_only_mppt_tracking(updater, tm);

        TEST_ASSERT_LESS_THAN(dac_upper_boundary + 1, output);
        TEST_ASSERT_GREATER_THAN(dac_lower_boundary - 1, output);
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
uint12_t clamp_value(float value, float lower_limit, float upper_limit) {
    uint16_t output = round(value);

    if (value > upper_limit) {
        output = upper_limit;
    }
    if (value < lower_limit) {
        output = lower_limit;
    }
    return output;
}

/*!
 * Function to simulate sinus-like Voltage-vs-Power curve.
 * @param solar_voltage 12-bit solar voltage.
 * @return Calculated solar power at specified solar voltage.
 */
uint12_t mock_PVCurveSinus(uint12_t solar_voltage) {
    int sine =
        round(sin(((solar_voltage - 1300) / 4096.0) * M_PI * 1.5) * 1100) + 1000;
    uint16_t output_power = clamp_value(sine, 0, 4095);
    return output_power;
}

/*!
 * Function to find MPP on a Voltage-vs-Power curve.
 * @return Power value at MPP.
 */
uint12_t mock_PVCurve_FindMppPower() {
    uint16_t mpp_power = 0;

    for (uint16_t solar_voltage = 0; solar_voltage < 4095; solar_voltage += 8) {
        uint16_t solar_power = mock_PVCurveSinus(solar_voltage);

        if (solar_power > mpp_power) {
            mpp_power = solar_power;
        }
    }

    LOG_DEBUG("MPP at solar_power = %u", mpp_power);
    return mpp_power;
}

void test_MpptUpdater_MppTracking_SinusPVCurve_StartingPointSweep() {
    uint12_t dac_set   = 0;
    uint12_t mpp_power = mock_PVCurve_FindMppPower();

    Telemetry::SingleMpptChannel tm;
    tm.solar_current  = 4000;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    for (uint16_t starting_point = dac_lower_boundary;
         starting_point <= dac_upper_boundary;
         starting_point += 8) {
        MpptUpdater<eps::MpptSettings::Y> updater(starting_point);

        for (int i = 0; i < 1000; i++) {
            updater.update_tm(tm);
            dac_set = mock_only_mppt_tracking(updater, tm);

            tm.solar_voltage = mock_PVCurveSinus(dac_set) + ((rand() % 3) - 1);
        }

        TEST_ASSERT_LESS_THAN(dac_upper_boundary, dac_set);
        TEST_ASSERT_GREATER_THAN(dac_lower_boundary, dac_set);

        TEST_ASSERT_LESS_THAN(mpp_power + max_mpp_error, tm.solar_voltage);
        TEST_ASSERT_GREATER_THAN(mpp_power - max_mpp_error, tm.solar_voltage);
    }

    LOG_DEBUG("MppTracking mode found MPP at: %u", tm.solar_voltage);
}

void test_MpptUpdater_MppTracking_SinusPVCurve_SolarCurrentSweep() {
    uint12_t mpp_power = mock_PVCurve_FindMppPower();

    Telemetry::SingleMpptChannel tm;
    tm.solar_voltage  = 10;
    tm.output_voltage = 0;

    for (uint16_t solar_current = 1; solar_current <= 4095; solar_current += 16) {
        MpptUpdater<eps::MpptSettings::Y> updater(3000);
        uint12_t dac_set = 0;
        tm.solar_current = solar_current;

        for (int i = 0; i < 1000; i++) {
            updater.update_tm(tm);
            dac_set = mock_only_mppt_tracking(updater, tm);

            tm.solar_voltage = mock_PVCurveSinus(dac_set) + ((rand() % 3) - 1);
        }

        TEST_ASSERT_LESS_THAN(dac_upper_boundary, dac_set);
        TEST_ASSERT_GREATER_THAN(dac_lower_boundary, dac_set);

        TEST_ASSERT_LESS_THAN(mpp_power + max_mpp_error, tm.solar_voltage);
        TEST_ASSERT_GREATER_THAN(mpp_power - max_mpp_error, tm.solar_voltage);
    }

    LOG_DEBUG("MppTracking mode found MPP at: %u", tm.solar_voltage);
}

void test_MpptUpdater_DacSweepDown() {
    for (uint16_t starting_point = dac_lower_boundary;
         starting_point <= dac_upper_boundary;
         starting_point++) {
        MpptUpdater<eps::MpptSettings::Y> updater(starting_point);

        for (int i = 0; i < 300; i++) {
            auto output = mock_only_sweep_down(updater);
            TEST_ASSERT_LESS_THAN(dac_upper_boundary + 1, output);
            TEST_ASSERT_GREATER_THAN(dac_lower_boundary - 1, output);
        }

        auto output = mock_only_sweep_down(updater);
        TEST_ASSERT_EQUAL(dac_lower_boundary, output);
    }
}

void test_MpptUpdater_DacSweepUp() {
    for (uint16_t starting_point = dac_lower_boundary;
         starting_point <= dac_upper_boundary;
         starting_point++) {
        MpptUpdater<eps::MpptSettings::Y> updater(starting_point);

        for (int i = 0; i < 300; i++) {
            auto output = mock_only_sweep_up(updater);
            TEST_ASSERT_LESS_THAN(dac_upper_boundary + 1, output);
            TEST_ASSERT_GREATER_THAN(dac_lower_boundary - 1, output);
        }

        auto output = mock_only_sweep_up(updater);
        TEST_ASSERT_EQUAL(dac_upper_boundary, output);
    }
}

void test_MpptUpdater_MppRestart() {
    MpptUpdater<eps::MpptSettings::Y> updater(127);

    updater.mpp_restart();
    TEST_ASSERT_EQUAL(dac_upper_boundary, updater.dac);
}

void test_MpptUpdater_CalculatePower() {
    MpptUpdater<eps::MpptSettings::Y> updater(dac_lower_boundary);
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

    UnityEnd();
}
