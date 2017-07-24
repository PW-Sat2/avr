#ifndef PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_IOMAP_H_
#define PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_IOMAP_H_

#include <hal/hal>
#include "RadfetAdc.h"
#include "Watchdog.h"

namespace Mux {
using A0 = hal::DigitalIO::GPIO<42>;
using A1 = hal::DigitalIO::GPIO<40>;
using A2 = hal::DigitalIO::GPIO<41>;
using EN = hal::DigitalIO::GPIO<43>;
}  // namespace Mux

namespace adc128_interface {
using cs = hal::DigitalIO::GPIO<9>;

using spi = hal::SPI::Hardware<cs,
                               hal::SPI::HardwareClockDivisor::SPIHard_DIV_16,
                               hal::SPI::Polarity::idle_low,
                               hal::SPI::Phase::trailing_sample,
                               hal::SPI::DataOrder::MSB_first>;
}  // namespace adc128_interface

using adg708 = hal::devices::ADG708::ADG708<Mux::A0, Mux::A1, Mux::A2, Mux::EN>;
using adc128 = hal::devices::ADC128::ADC128<adc128_interface::spi>;

using watchdog_pin      = hal::DigitalIO::GPIO<44>;
using tps3813           = hal::devices::TPS3813<watchdog_pin, 10>;
using external_watchdog = pld::devices::Watchdog<tps3813>;

using interrupt = hal::DigitalIO::GPIO<10>;


using watchdog_pin = hal::DigitalIO::GPIO<44>;
using tps3813      = hal::devices::TPS3813<watchdog_pin, 10>;
using interrupt    = hal::DigitalIO::GPIO<10>;


namespace radfet {
using pin_en = hal::DigitalIO::GPIO<35>;

namespace adc {
using pin_ss   = hal::DigitalIO::GPIO<33>;
using pin_mosi = hal::DigitalIO::GPIO<30>;
using pin_miso = hal::DigitalIO::GPIO<31>;
using pin_sck  = hal::DigitalIO::GPIO<26>;
using pin_drdy = hal::DigitalIO::GPIO<32>;

using spi_soft = hal::SPI::
    Software<pin_mosi, pin_miso, pin_sck, pin_ss, hal::SPI::Polarity::idle_low, hal::SPI::Phase::trailing_sample>;

using ad7714 =
    pld::devices::AD7714::RadfetAdc<pld::devices::AD7714::AD7714<spi_soft, pin_drdy>,
                                    pld::devices::Watchdog<tps3813>>;

}  // namespace adc

namespace mux {
using pin_en = hal::DigitalIO::GPIO<13>;
using pin_a0 = hal::DigitalIO::GPIO<14>;
using pin_a1 = hal::DigitalIO::GPIO<15>;

using mos_mux = hal::devices::ADG709::ADG709<pin_a0, pin_a1, pin_en>;

using pin_high_side_current_mux = hal::DigitalIO::GPIO<25>;
using high_side_current_mux =
    hal::devices::ADG849::ADG849<pin_high_side_current_mux>;
}  // namespace mux

}  // namespace radfet

#endif  // PLD_PLD_LIBS_HARDWARE_INCLUDE_HARDWARE_IOMAP_H_
