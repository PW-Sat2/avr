#include <avr/interrupt.h>
#include <logger.h>
#include <hal/hal>
#include "hal/libs/terminal/terminal.h"

#include "hardware/mock.h"

namespace pld {
namespace debug {

using namespace std;
using namespace hardware::mock;

void set_adc(std::uint8_t argc, char* argv[]) {
    if (argc != 2)
        return;

    uint8_t channel = atoi(argv[0]);
    uint16_t value  = atoi(argv[1]);

    assert(channel < adc_channels.size());
    adc_channels[channel] = value;
}

bool mock_ = false;

void set_mock(std::uint8_t argc, char* argv[]) {
    if (argc != 1)
        return;

    if (argv[0][0] == '1') {
        mock_ = true;
        LOG_INFO("Mock");
    } else {
        mock_ = false;
        LOG_INFO("Real HW");
    }
}

Terminal terminal;

ISR(USART1_RX_vect) {
    char now = UDR1;

    static char buf[20];
    static hal::libs::FIFO<char> fifo{buf, 20};

    if (now == '\n') {
        fifo.append(0);
        terminal.HandleCommand(buf + 1);
        fifo.flush();
    } else {
        fifo.append(now);
    }
}

bool mock() {
    return mock_;
}

void init() {
    static TerminalCommandDescription cmds[] = {{"adc", set_adc},
                                                {"mock", set_mock}};

    terminal.SetCommandList(cmds);
}

}  // namespace debug
}  // namespace pld
