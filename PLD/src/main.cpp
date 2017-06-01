#define __ASSERT_USE_STDERR
#include <assert.h>

#include <hal/hal>
#include <ObcInterface/ObcInterface.h>

using namespace hal;
using namespace hal::libs;

struct Data {
    struct SunsRef {
        array<uint16_t, 5> voltages;
    };

    struct Photodiodes {
        uint16_t X_p;
        uint16_t X_m;
        uint16_t Y_p;
        uint16_t Y_m;
    };

    struct Temperatures {
        uint16_t supply;
        uint16_t X_p;
        uint16_t X_m;
        uint16_t Y_p;
        uint16_t Y_m;
        uint16_t sads;
        uint16_t sail;
        uint16_t gyro;
    };

    struct Voltages {
        uint16_t int_3v3d;
        uint16_t obc_3v3d;
    };

    struct Radfet {
        uint32_t temperature;
        array<uint32_t, 3> vth;
    };

    uint8_t status;
    SunsRef suns_ref;
    Photodiodes photodiodes;
    Temperatures temperatures;
    Voltages voltages;
    Radfet radfet;
};
static_assert(sizeof(Data) == 55, "Incorrect size of Data structure (padding?)");


void CommandCallback(span<uint8_t> ) {
}

::drivers::ObcInterface<0x1A, CommandCallback, 4, Data> obc;

ISR(TWI_vect) {
    obc.process_interrupt();
}

Data data;

int main() {
    Serial0.init(115200, hal::STDIO::ENABLE);

    obc.init(&data);
}
