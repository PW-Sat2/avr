#ifndef PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
#define PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_

#include <util/atomic.h>
#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <hal/hal>
#include "atomic.h"


namespace pld {

struct Telemetry {
    struct SunsRef {
        std::array<std::uint16_t, 5> voltages;
    };

    struct Temperatures {
        std::uint16_t supply;
        std::uint16_t Xp;
        std::uint16_t Xn;
        std::uint16_t Yp;
        std::uint16_t Yn;
        std::uint16_t sads;
        std::uint16_t sail;
        std::uint16_t cam_nadir;
        std::uint16_t cam_wing;
    };

    struct Photodiodes {
        std::uint16_t Xp;
        std::uint16_t Xn;
        std::uint16_t Yp;
        std::uint16_t Yn;
    };

    struct Housekeeping {
        std::uint16_t int_3v3d;
        std::uint16_t obc_3v3d;
    };

    struct Radfet {
        struct Status {
            enum class Fields {
                On                 = 0,
                TimeoutVth0        = 1,
                TimeoutVth1        = 2,
                TimeoutVth2        = 3,
                TimeoutTemperature = 4,
                MeasurementDone    = 5,
                LCL_3V3_error      = 6,
                LCL_5V_error       = 7,

                Length
            };


            void set_status(Fields field, bool value) {
                static_assert(num(Fields::Length) <= 8, "One byte status allowed!");

                std::bitset<num(Fields::Length)> bitset(this->status);

                bitset[num(field)] = value;

                status = static_cast<uint8_t>(bitset.to_ulong());
            }

         private:
            std::uint8_t status;
        };


        struct Measurement {
            std::uint32_t temperature;
            std::array<std::uint32_t, 3> vth;
        };

        Status status;
        avr::Atomic<Measurement> measurement;
    };

    uint8_t who_am_i;
    avr::Atomic<SunsRef> suns_ref;
    avr::Atomic<Temperatures> temperatures;
    avr::Atomic<Photodiodes> photodiodes;
    avr::Atomic<Housekeeping> housekeeping;
    Radfet radfet;

    void init() {
        std::memset(this, 0xFF, sizeof(pld::Telemetry));
    }
};
static_assert(sizeof(Telemetry) == 58,
              "Incorrect size of Telemetry structure (padding?)");
static_assert(std::is_pod<Telemetry>::value, "POD");


}  // namespace pld

#endif  // PLD_PLD_LIBS_TELEMETRY_INCLUDE_TELEMETRY_TELEMETRY_H_
