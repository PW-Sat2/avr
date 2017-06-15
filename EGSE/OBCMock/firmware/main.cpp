#include <hal/board.h>
#include <hal/libs/terminal/terminal.h>
#include <hal/hal>

using namespace hal;

constexpr static auto pin_INT0 = 32;

using pin = hal::DigitalIO::GPIO<pin_INT0>;

using i2c = hal::I2C::Hardware;

static std::array<uint8_t, 300> array;

void help(uint8_t, char* []) {
    printf("Available commands:\n");
    printf("  w - Write passed bytes. w $address $data... (for example w 10 1 "
           "2 3 4 5)\n");
    printf(
        "  r - Read n bytes. r $address $nr_of_bytes (for example r 10 5)\n");
    printf("  wr - Write and read. wr $address $data... $nr_of_bytes (for "
           "example wr 10 1 2 3 15)\n");
}

void write(uint8_t argc, char* argv[]) {
    if (argc == 0) {
        return;
    }

    uint8_t addr = atoi(argv[0]);

    for (int i = 0; i < argc - 1; ++i) {
        array[i] = atoi(argv[i + 1]);
    }

    i2c::write(addr, gsl::make_span(array.data(), argc - 1));
}

void read(uint8_t argc, char* argv[]) {
    if (argc < 2) {
        return;
    }

    uint8_t addr  = atoi(argv[0]);
    uint8_t bytes = atoi(argv[1]);
    if (bytes == 0 || bytes > array.size()) {
        return;
    }

    auto data = gsl::make_span(array.data(), bytes);
    i2c::read(addr, data);

    for (auto i : data) {
        printf("%d ", i);
    }
}

void write_read(uint8_t argc, char* argv[]) {
    if (argc < 2) {
        return;
    }

    uint8_t addr         = atoi(argv[0]);
    uint8_t bytesToWrite = argc - 2;
    uint8_t bytesToRead  = atoi(argv[argc - 1]);

    static std::array<uint8_t, 10> WriteArray;
    if (bytesToWrite > WriteArray.size()) {
        return;
    }

    for (int i = 0; i < bytesToWrite; ++i) {
        WriteArray[i] = atoi(argv[i + 1]);
    }

    auto write_data = gsl::make_span(WriteArray.data(), bytesToWrite);
    auto read_data  = gsl::make_span(array.data(), bytesToRead);

    i2c::write_read(addr, write_data, read_data);

    for (auto x : read_data) {
        printf("%d ", x);
    }
}

void pin_read(uint8_t, char* []) {
    if (pin::read()) {
        printf("1");
    } else {
        printf("0");
    }
}

libs::FIFO_data<uint8_t, 50> fifo;

void events(uint8_t, char* []) {
    while (fifo.getLength()) {
        printf("%d", fifo.get());
    }
}

void led(uint8_t argc, char* argv[]) {
    if (argc < 1)
        return;
    bsp::LED::write(atoi(argv[0]));
}

TerminalCommandDescription tcs[] = {{"?", help},
                                    {"w", write},
                                    {"r", read},
                                    {"wr", write_read},
                                    {"pin", pin_read},
                                    {"ev", events},
                                    {"led", led}};

Terminal terminal;


ISR(USART_RX_vect) {
    uint8_t now = UDR0;

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


ISR(INT0_vect) {
    if (pin::read()) {
        //        bsp::LED::on();
        fifo.append(1);
    } else {
        //        bsp::LED::off();
        fifo.append(0);
    }
}

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
    Serial0.enable_rx_interrupt();

    bsp::LED::init();

    i2c::init<100000>();
    i2c::enable_internal_pullups();

    pin::init(hal::DigitalIO::Mode::INPUT_PULLUP);
    using line =
        hal::DigitalIO::ExternalInterrupt::Line<0, hal::DigitalIO::ExternalInterrupt::Mode::change>;
    line::enable();
    sei();

    terminal.SetCommandList(tcs);

    Watchdog::enable(Watchdog::Period::p2000ms);

    while (1) {
        Watchdog::kick();
    }
}
