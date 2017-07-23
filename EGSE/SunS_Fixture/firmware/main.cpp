#include <hal/libs/terminal/terminal.h>
#include <hal/hal>

using namespace hal;

constexpr static auto pin_INT0 = 16;

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


TerminalCommandDescription tcs[] = {{"?", help},
                                    {"w", write},
                                    {"r", read},
                                    {"wr", write_read},
                                    {"pin", pin_read},
                                    {"ev", events}};

Terminal terminal;


ISR(USART1_RX_vect) {
    uint8_t now = UDR1;

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


ISR(PCINT3_vect) {
    if (pin::read()) {
        fifo.append(1);
    } else {
        fifo.append(0);
    }
}

int main() {
    Serial1.init(38400);
    Serial1.redirect_stdio();
    Serial1.redirect_stderr();
    Serial1.enable_rx_interrupt();

    printf("Init!\n");


    i2c::init<100000>();
    i2c::enable_internal_pullups();

    pin::init(hal::DigitalIO::Mode::INPUT);
    PCMSK3 |= (1 << PCINT31);  // pin16
    PCICR |= (1 << PCIE3);
    sei();

    terminal.SetCommandList(tcs);

    Watchdog::disable();

    while (1) {
    }
}
