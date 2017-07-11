#include <algorithm>
#include <hal/hal>
#include "unity.h"

#include "hardware/interface.h"
#include "telemetry/telemetry.h"

#include "commands/HouseKeeping.h"
#include "commands/PT1000.h"
#include "commands/Photodiodes.h"
#include "commands/RadFET.h"
#include "commands/SunSref.h"

using namespace std;
using namespace hal;
using namespace hal::libs;

std::array<uint16_t, 20> adc_mock_data;
FIFO_data<uint8_t, 20> commands;

class MockHW : public pld::hardware::Interface {
 public:
    MockHW() {
        radfet_called_method = RadfetCallType::None;
    }
    void init() override {
        TEST_FAIL_MESSAGE("init");
    }
    void read_adc(std::initializer_list<ChannelDescriptor> channels) override {
        for (auto ch : channels) {
            auto channel = static_cast<uint8_t>(ch.channel);
            commands.append(channel);
            *ch.data = adc_mock_data[channel];
        }
    }


    enum class RadfetCallType {
        None,
        On,
        Read,
        Off,
    };

    RadfetCallType radfet_called_method;
    pld::Telemetry::Radfet radfet_telemetry;

    void radfet_on() override {
        if (radfet_called_method == RadfetCallType::None) {
            radfet_called_method = RadfetCallType::On;
        }
    }

    pld::Telemetry::Radfet radfet_read() override {
        if (radfet_called_method == RadfetCallType::None) {
            radfet_called_method = RadfetCallType::Read;
        }
        return radfet_telemetry;
    }

    void radfet_off() override {
        if (radfet_called_method == RadfetCallType::None) {
            radfet_called_method = RadfetCallType::Off;
        }
    }

    void external_watchdog_kick() override {
        TEST_FAIL_MESSAGE("watchdog kicked");
    }

    void obc_interrupt_set() override {
        TEST_FAIL_MESSAGE("interrupt set");
    }

    void obc_interrupt_reset() override {
        TEST_FAIL_MESSAGE("interrupt reset");
    }
};

pld::Telemetry telemetry;
MockHW hw;
pld::hardware::HardwareProvider hw_ptr;

void set_adc_mock(pld::hardware::AnalogChannel channel, uint16_t value) {
    adc_mock_data[static_cast<uint8_t>(channel)] = value;
}

void check_readouts(std::initializer_list<pld::hardware::AnalogChannel> channels) {
    TEST_ASSERT_EQUAL(channels.size(), commands.getLength());
    while (commands.getLength() > 0) {
        auto now = commands.get();

        bool ok = false;
        for (auto req : channels) {
            if (static_cast<uint8_t>(req) == now) {
                ok = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(ok);
    }
}

void check_telemetry_empty() {
    std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&telemetry);
    gsl::span<uint8_t> view(ptr, sizeof(pld::Telemetry));

    TEST_ASSERT_TRUE(std::all_of(
        view.begin(), view.end(), [](uint8_t i) { return i == 0xFF; }));
}

bool operator==(const pld::Telemetry::Radfet& rhs,
                const pld::Telemetry::Radfet& lhs) {
    return memcmp(&rhs, &lhs, sizeof(pld::Telemetry::Radfet)) == 0;
}

template<typename T>
void reset_telemetry_field(T& t) {
    static_assert(std::is_pod<T>::value, "Structure has to be POD!");
    memset(&t, 0xFF, sizeof(T));
}

using namespace pld::hardware;

void test_commands_HouseKeeping() {
    memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    set_adc_mock(AnalogChannel::HouseKeeping_3V3d, 12345);
    set_adc_mock(AnalogChannel::HouseKeeping_3V3_OBC, 52687);

    pld::commands::HouseKeeping().invoke(telemetry, hw, {});
    check_readouts({AnalogChannel::HouseKeeping_3V3d,
                    AnalogChannel::HouseKeeping_3V3_OBC});

    pld::Telemetry::Housekeeping readed = telemetry.housekeeping;
    TEST_ASSERT_EQUAL_UINT16(12345, readed.int_3v3d);
    TEST_ASSERT_EQUAL_UINT16(52687, readed.obc_3v3d);

    set_adc_mock(AnalogChannel::HouseKeeping_3V3d, 31204);
    set_adc_mock(AnalogChannel::HouseKeeping_3V3_OBC, 41578);

    pld::commands::HouseKeeping().invoke(telemetry, hw, {});
    check_readouts({AnalogChannel::HouseKeeping_3V3d,
                    AnalogChannel::HouseKeeping_3V3_OBC});

    readed = telemetry.housekeeping;
    TEST_ASSERT_EQUAL_UINT16(31204, readed.int_3v3d);
    TEST_ASSERT_EQUAL_UINT16(41578, readed.obc_3v3d);
}

void test_commands_Photodiodes() {
    memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    set_adc_mock(AnalogChannel::PhotodiodeXp, 60001);
    set_adc_mock(AnalogChannel::PhotodiodeXn, 60002);
    set_adc_mock(AnalogChannel::PhotodiodeYp, 60003);
    set_adc_mock(AnalogChannel::PhotodiodeYn, 60004);

    pld::commands::Photodiodes().invoke(telemetry, hw, {});
    check_readouts({AnalogChannel::PhotodiodeXp,
                    AnalogChannel::PhotodiodeXn,
                    AnalogChannel::PhotodiodeYp,
                    AnalogChannel::PhotodiodeYn});

    pld::Telemetry::Photodiodes readed = telemetry.photodiodes;
    TEST_ASSERT_EQUAL_UINT16(60001, readed.Xp);
    TEST_ASSERT_EQUAL_UINT16(60002, readed.Xn);
    TEST_ASSERT_EQUAL_UINT16(60003, readed.Yp);
    TEST_ASSERT_EQUAL_UINT16(60004, readed.Yn);
}

void test_commands_SunSRef() {
    memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    set_adc_mock(AnalogChannel::SunSRef_V0, 50001);
    set_adc_mock(AnalogChannel::SunSRef_V1, 50002);
    set_adc_mock(AnalogChannel::SunSRef_V2, 50003);
    set_adc_mock(AnalogChannel::SunSRef_V3, 50004);
    set_adc_mock(AnalogChannel::SunSRef_V4, 50005);

    pld::commands::SunSRef().invoke(telemetry, hw, {});
    check_readouts({AnalogChannel::SunSRef_V0,
                    AnalogChannel::SunSRef_V1,
                    AnalogChannel::SunSRef_V2,
                    AnalogChannel::SunSRef_V3,
                    AnalogChannel::SunSRef_V4});

    pld::Telemetry::SunsRef readed = telemetry.suns_ref;
    TEST_ASSERT_EQUAL_UINT16(50001, readed.voltages[0]);
    TEST_ASSERT_EQUAL_UINT16(50002, readed.voltages[1]);
    TEST_ASSERT_EQUAL_UINT16(50003, readed.voltages[2]);
    TEST_ASSERT_EQUAL_UINT16(50004, readed.voltages[3]);
    TEST_ASSERT_EQUAL_UINT16(50005, readed.voltages[4]);
}

void test_commands_Temperatures() {
    memset(&telemetry, 0xFF, sizeof(pld::Telemetry));

    set_adc_mock(AnalogChannel::TemperatureSupply, 40001);
    set_adc_mock(AnalogChannel::TemperatureCamNadir, 40002);
    set_adc_mock(AnalogChannel::TemperatureCamWing, 40003);
    set_adc_mock(AnalogChannel::TemperatureSail, 40004);
    set_adc_mock(AnalogChannel::TemperatureSADS, 40005);
    set_adc_mock(AnalogChannel::TemperatureXp, 40006);
    set_adc_mock(AnalogChannel::TemperatureXn, 40007);
    set_adc_mock(AnalogChannel::TemperatureYp, 40008);
    set_adc_mock(AnalogChannel::TemperatureYn, 40009);

    pld::commands::PT1000().invoke(telemetry, hw, {});
    check_readouts({
        AnalogChannel::TemperatureSupply,
        AnalogChannel::TemperatureCamNadir,
        AnalogChannel::TemperatureCamWing,
        AnalogChannel::TemperatureSail,
        AnalogChannel::TemperatureSADS,
        AnalogChannel::TemperatureXp,
        AnalogChannel::TemperatureXn,
        AnalogChannel::TemperatureYp,
        AnalogChannel::TemperatureYn,
    });

    pld::Telemetry::Temperatures readed = telemetry.temperatures;
    TEST_ASSERT_EQUAL_UINT16(40001, readed.supply);
    TEST_ASSERT_EQUAL_UINT16(40002, readed.cam_nadir);
    TEST_ASSERT_EQUAL_UINT16(40003, readed.cam_wing);
    TEST_ASSERT_EQUAL_UINT16(40004, readed.sail);
    TEST_ASSERT_EQUAL_UINT16(40005, readed.sads);
    TEST_ASSERT_EQUAL_UINT16(40006, readed.Xp);
    TEST_ASSERT_EQUAL_UINT16(40007, readed.Xn);
    TEST_ASSERT_EQUAL_UINT16(40008, readed.Yp);
    TEST_ASSERT_EQUAL_UINT16(40009, readed.Yn);
}

void test_radfet_on_invoke() {
    hw = MockHW();

    pld::commands::RadFET_On().invoke(telemetry, hw, {});

    TEST_ASSERT_EQUAL(MockHW::RadfetCallType::On, hw.radfet_called_method);
}

void test_radfet_on_telemetry() {
    telemetry.init();
    hw = MockHW();

    pld::commands::RadFET_On().invoke(telemetry, hw, {});

    check_telemetry_empty();
}

void test_commands_radfet_measure_invoke() {
    hw = MockHW();

    pld::commands::RadFET_Measure().invoke(telemetry, hw, {});

    TEST_ASSERT_EQUAL(MockHW::RadfetCallType::Read, hw.radfet_called_method);
}

void test_commands_radfet_measure_telemetry() {
    telemetry.init();
    hw                              = MockHW();
    hw.radfet_telemetry.vth         = {0xB00BA5, 0xBEEFED, 0xBAAAAD};
    hw.radfet_telemetry.temperature = 0xC0010F;

    pld::commands::RadFET_Measure().invoke(telemetry, hw, {});

    TEST_ASSERT_TRUE(hw.radfet_telemetry ==
                     static_cast<pld::Telemetry::Radfet>(telemetry.radfet));

    reset_telemetry_field(telemetry.radfet);
    check_telemetry_empty();
}

void test_radfet_off_invoke() {
    hw = MockHW();

    pld::commands::RadFET_Off().invoke(telemetry, hw, {});

    TEST_ASSERT_EQUAL(MockHW::RadfetCallType::Off, hw.radfet_called_method);
}

void test_radfet_off_telemetry() {
    telemetry.init();
    hw = MockHW();

    pld::commands::RadFET_Off().invoke(telemetry, hw, {});

    check_telemetry_empty();
}

void test_commands() {
    UnityBegin("");
    RUN_TEST(test_commands_HouseKeeping);
    RUN_TEST(test_commands_Photodiodes);
    RUN_TEST(test_commands_SunSRef);
    RUN_TEST(test_commands_Temperatures);

    RUN_TEST(test_radfet_on_invoke);
    RUN_TEST(test_radfet_on_telemetry);

    RUN_TEST(test_commands_radfet_measure_invoke);
    RUN_TEST(test_commands_radfet_measure_telemetry);

    RUN_TEST(test_radfet_off_invoke);
    RUN_TEST(test_radfet_off_telemetry);

    UnityEnd();
}
