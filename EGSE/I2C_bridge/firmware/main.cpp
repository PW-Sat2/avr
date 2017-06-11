#include <hal/libs/terminal/terminal.h>
#include <hal/hal>

using namespace hal;

hal::I2C::Hardware i2c;

static std::array<uint8_t, 300> array;

void help(uint8_t, char* []) {
    printf("Available commands:\n");
    printf("  w - Write passed bytes. w $address $data... (for example w 10 1 "
           "2 3 4 5)\n");
    printf(
        "  r - Read n bytes. r $address $nr_of_bytes (for example r 10 5)\n");
    printf("  wr - Write and read. wr $address $data... $nr_of_bytes (for "
           "example r 10 1 2 3 7)\n");
}

void write(uint8_t argc, char* argv[]) {
    if (argc == 0) {
        return;
    }

    uint8_t addr = atoi(argv[0]);

    for (int i = 0; i < argc - 1; ++i) {
        array[i] = atoi(argv[i + 1]);
    }

    //    printf("W [%d] %d bytes: ", addr, argc-1);
    //    for(int i = 0; i < argc-1; ++i) {
    //        printf("%d ", array[i]);
    //    }
    i2c.write(addr, gsl::make_span(array.data(), argc - 1));
}

void read(uint8_t argc, char* argv[]) {
    if (argc < 2) {
        return;
    }

    uint8_t addr  = atoi(argv[0]);
    uint8_t bytes = atoi(argv[1]);

    //    printf("R [%d] %d bytes: ", addr, bytes);

    auto data = gsl::make_span(array.data(), bytes);
    i2c.read(addr, data);

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
    for (int i = 0; i < argc - 2; ++i) {
        WriteArray[i] = atoi(argv[i + 1]);
    }

    auto write_data = gsl::make_span(WriteArray.data(), bytesToWrite);
    auto read_data  = gsl::make_span(array.data(), bytesToRead);

    //    printf("WR [%d] %d bytes [ ", addr, bytesToWrite);
    //    for(auto x: write_data) {
    //        printf("%d ", x);
    //    }
    //    printf("] and reading %d: ", bytesToRead);
    i2c.writeRead(addr, write_data, read_data);

    for (auto x : read_data) {
        printf("%d ", x);
    }
}

TerminalCommandDescription tcs[] = {{"?", help},
                                    {"w", write},
                                    {"r", read},
                                    {"wr", write_read}};

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

int main() {
    Serial0.init(38400);
    Serial0.redirect_stdio();
    Serial0.redirect_stderr();
    Serial0.enable_rx_interrupt();

    i2c.init<100000>();
    i2c.enable_internal_pullups();

    sei();

    terminal.SetCommandList(tcs);

    while (1) {
    }
}
