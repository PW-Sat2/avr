#ifndef PLD_SRC_HW_H_
#define PLD_SRC_HW_H_

#include <hal/hal>

struct PLDHardware {
    enum class AnalogChannel : std::uint8_t {
        SunSRef_V0,
        SunSRef_V1,
        SunSRef_V2,
        SunSRef_V3,
        SunSRef_V4,
        Photodiode_A,
        Photodiode_B,
        Photodiode_C,
        Photodiode_D,
        TemperatureSupply,
        HouseKeeping_3V3d,
        HouseKeeping_3V3_OBC,
        TemperatureSail,
        TemperatureSADS,
        TemperatureCamWing,
        TemperatureCamNadir,
        TemperatureXn,
        TemperatureXp,
        TemperatureYn,
        TemperatureYp,
    };


    class Mux : public hal::ADG708 {
     public:
        Mux() : ADG708(A0, A1, A2, EN) {
        }

     private:
        hal::DigitalIO::GPIO<42> A0;
        hal::DigitalIO::GPIO<40> A1;
        hal::DigitalIO::GPIO<41> A2;
        hal::DigitalIO::GPIO<43> EN;
    };

    class ADC128 : public hal::drivers::ADC128 {
     public:
        ADC128() : hal::drivers::ADC128(spi), spi(cs) {
        }

        void init() {
            spi.init();
        }

     private:
        hal::DigitalIO::GPIO<9> cs;

        // [todo]: correct SPI settings
        hal::SPI::Hardware<hal::SPI::HardwareClockDivisor::SPIHard_DIV_4,
                           hal::SPI::Polarity::idle_high,
                           hal::SPI::Phase::leading_sample,
                           hal::SPI::DataOrder::MSB_first>
            spi;
    };

    struct RadFET {
        class Mux : public hal::ADG709 {
         public:
            Mux() : ADG709(A0, A1, EN) {
            }

         private:
            hal::DigitalIO::GPIO<14> A0;
            hal::DigitalIO::GPIO<12> A1;
            hal::DigitalIO::GPIO<13> EN;
        };

        class AD7714 : public hal::AD7714 {
         public:
            AD7714() : hal::AD7714(spi, DRDY), spi(MOSI, MISO, SCK, CS) {
            }

            void init() {
                spi.init();
                hal::AD7714::init();
            }

         private:
            hal::DigitalIO::GPIO<26> SCK;
            hal::DigitalIO::GPIO<30> MOSI;
            hal::DigitalIO::GPIO<31> MISO;
            hal::DigitalIO::GPIO<32> DRDY;
            hal::DigitalIO::GPIO<33> CS;

         public:
            hal::SPI::Software<hal::SPI::Polarity::idle_high, hal::SPI::Phase::leading_sample> spi;
        };

        Mux mux;
        AD7714 ad7714;

        hal::DigitalIO::GPIO<25> Temp_MUX;
        hal::DigitalIO::GPIO<35> enable;

        void init() {
            mux.init();
            ad7714.init();
        }
    };

    hal::DigitalIO::GPIO<10> interrupt;
    hal::DigitalIO::GPIO<44> watchdog;

    Mux mux;
    ADC128 adc128;
    RadFET radFET;


    void init();
    void standby();
    uint16_t read_adc_and_change_channel(AnalogChannel next_channel);
};

extern PLDHardware hw;

#endif  // PLD_SRC_HW_H_
