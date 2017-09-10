#ifndef EPS_A_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
#define EPS_A_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_

#include <hal/hal>
#include "PowerCycleCounter.h"
#include "telemetry.h"

namespace eps {

/*!
 * Telemetry updater module. Updates telemetry values when ivoked.
 * This is split in two parts: general (1 second interval) and MPPT (33 ms)
 * @tparam telemetry Telemetry to update
 * @tparam Mux ADG709 mux type (device from IOMap)
 * @tparam InternalADC InternalADC from hal (hal::Analog::AnalogGPIO)
 * @tparam Mppt MPPT structure to use (from IOMap)
 * @tparam LclCommander LCL commander to use to get statuses of LCLs
 */
template<eps::Telemetry& telemetry,
         template<hal::Analog::InternalADC::Input> typename InternalADC,
         typename Eps>
class TelemetryUpdater : hal::libs::PureStatic {
 public:
    /*!
     * Update "General" field in telemetry table.
     * Reads from physical devices and
     * stores result atomically in telemetry table.
     */
    static void update_general() {
        eps::Telemetry::General tm = telemetry.general;

        Eps::IOMap::Mux::select(MuxCh::S1);
        tm.distribution.vbat_current      = Adc<AdcCh::ADC0>::read();
        tm.distribution.dcdc_5v_current   = Adc<AdcCh::ADC1>::read();
        tm.battery_controller.temperature = Adc<AdcCh::ADC3>::read();

        Eps::IOMap::Mux::select(MuxCh::S2);
        tm.distribution.vbat_voltage         = Adc<AdcCh::ADC0>::read();
        tm.distribution.dcdc_5v_voltage      = Adc<AdcCh::ADC1>::read();
        tm.controller_a.temperature          = Adc<AdcCh::ADC2>::read();
        tm.battery_controller.charge_current = Adc<AdcCh::ADC3>::read();

        Eps::IOMap::Mux::select(MuxCh::S3);
        tm.distribution.dcdc_3v3_current        = Adc<AdcCh::ADC0>::read();
        tm.dcdc_3v3_temperature                 = Adc<AdcCh::ADC1>::read();
        tm.controller_b.supply_voltage          = Adc<AdcCh::ADC2>::read();
        tm.battery_controller.discharge_current = Adc<AdcCh::ADC3>::read();

        Eps::IOMap::Mux::select(MuxCh::S4);
        tm.distribution.dcdc_3v3_voltage           = Adc<AdcCh::ADC0>::read();
        tm.dcdc_5v_temperature                     = Adc<AdcCh::ADC1>::read();
        tm.controller_a.supply_temperature         = Adc<AdcCh::ADC2>::read();
        tm.battery_controller.controller_a_voltage = Adc<AdcCh::ADC3>::read();

        auto power_cycles              = eps::power_cycle_counters::get();
        tm.controller_a.power_cycles   = power_cycles.all;
        tm.controller_a.safety_counter = power_cycles.safety;

        static uint32_t uptime = 0;
        tm.controller_a.uptime = uptime++;

        tm.distribution.lcl_state = Eps::LclCommander::on_status();
        tm.distribution.lcl_flagb = Eps::LclCommander::overcurrent_status();

        Eps::IOMap::BatteryTemperatureSensorA::Spi::init();
        tm.battery_pack.temperature_a =
            Eps::IOMap::BatteryTemperatureSensorA::Tmp121::read_raw();

        Eps::IOMap::BatteryTemperatureSensorB::Spi::init();
        tm.battery_pack.temperature_b =
            Eps::IOMap::BatteryTemperatureSensorB::Tmp121::read_raw();


        telemetry.general = tm;
    }

    /*!
     * Update all MPPT channels telemetry.
     */
    static void update_mppt() {
        Telemetry::AllMpptChannels tm = telemetry.mppt;

        update_mppt_channel<typename Eps::IOMap::Mppt::MpptX>(tm.mpptx);
        update_mppt_channel<typename Eps::IOMap::Mppt::MpptYp>(tm.mpptyp);
        update_mppt_channel<typename Eps::IOMap::Mppt::MpptYn>(tm.mpptyn);

        telemetry.mppt = tm;
    }

 private:
    using MuxCh = hal::devices::ADG709::Channel;

    template<hal::Analog::InternalADC::Input input>
    using Adc = InternalADC<input>;

    using AdcCh = hal::Analog::InternalADC::Input;

    using Adc124Ch = hal::devices::ADC124::Channel;

    template<typename MpptCh>
    static void update_mppt_channel(Telemetry::SingleMpptChannel& tm) {
        MpptCh::AdcSpi::init();

        MpptCh::Adc124::read_and_change_channel(Adc124Ch::IN0);

        tm.solar_current = MpptCh::Adc124::read_and_change_channel(Adc124Ch::IN1);
        tm.solar_voltage = MpptCh::Adc124::read_and_change_channel(Adc124Ch::IN2);
        tm.temperature = MpptCh::Adc124::read_and_change_channel(Adc124Ch::IN3);
        tm.output_voltage = MpptCh::Adc124::read_and_change_channel(Adc124Ch::IN3);
    }
};

}  // namespace eps

#endif  // EPS_A_EPS_LIBS_TELEMETRYUPDATER_TELEMETRYUPDATER_H_
