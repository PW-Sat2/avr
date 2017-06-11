#include <avr/interrupt.h>
#include <hal/libs/terminal/terminal.h>
#include <hal/hal>

#include <hardware/mock.h>

namespace pld {
namespace debug {

using namespace std;
using namespace hardware::mock;

void set_adc(std::uint8_t argc, char* argv[]) {
    if (argc != 2)
        return;

    uint8_t channel = atoi(argv[0]);
    uint32_t value  = atoi(argv[1]);

    assert(channel < adc_channels.size());
    adc_channels[channel] = value;
}

Terminal terminal;

ISR(USART0_RX_vect) {
    char now = UDR0;

    static char buf[100];
    static hal::libs::FIFO<char> fifo{buf, 100};

    if (now == '\n') {
        fifo.append(0);
        terminal.HandleCommand(buf + 1);
        fifo.flush();
    } else {
        fifo.append(now);
    }
}

void init() {
    static TerminalCommandDescription cmds[] = {{"adc", set_adc}};

    terminal.SetCommandList(cmds);
}

}  // namespace debug
}  // namespace pld
