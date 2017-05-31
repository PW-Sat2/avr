#include <hal/hal>

namespace drivers {

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
class ObcInterface {
 public:
    static_assert(i2c_address <= 0x7F, "I2C address should be 7-bit!");
    
    void init() {
        hal::DigitalIO::GPIO<hal::mcu::pin_sda> pin_sda;
        hal::DigitalIO::GPIO<hal::mcu::pin_scl> pin_scl;
        
        pin_sda.init(hal::DigitalIO::Interface::Mode::INPUT_PULLUP);
        pin_scl.init(hal::DigitalIO::Interface::Mode::INPUT_PULLUP);
        
        TWAR = (i2c_address << 1);
        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
    }
    
    void disable() {
        TWCR = 0;
    }
    
    static inline void set_memory(DataType *memory_) {
        memory = reinterpret_cast<uint8_t *>(memory_);
    }
 
    inline static void process_interrupt();

 private:
    static uint8_t rx_buffer[rx_max_length];
    static uint8_t rx_buffer_cnt;
    static constexpr uint8_t rx_buffer_cnt_max = rx_max_length;
    
    static uint8_t *memory;
    static uint8_t tx_buffer_cnt;
    static constexpr uint8_t tx_buffer_cnt_max = sizeof(DataType);
};

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
void ObcInterface<i2c_address, callback, rx_max_length, DataType>::process_interrupt() {
    uint8_t twsr = (TWSR & 0xF8);
    uint8_t twdr = TWDR;

    switch (twsr) {
        case TW_SR_SLA_ACK:  // 0x60
            rx_buffer_cnt = 0;
            break;
        
        case TW_SR_DATA_ACK:  // 0x80
            if (rx_buffer_cnt != rx_buffer_cnt_max) {
                rx_buffer[rx_buffer_cnt] = twdr;
                rx_buffer_cnt++;
            }
            break;
        
        case TW_SR_STOP:  // 0xA0
            callback(hal::libs::span<uint8_t>{rx_buffer, rx_buffer_cnt});
            break;
        
        case TW_ST_SLA_ACK:  // 0xA8
            tx_buffer_cnt = rx_buffer[0];
            TWDR = memory[0];
            break;
        
        case TW_ST_DATA_ACK:  // 0xB8
            if (tx_buffer_cnt < tx_buffer_cnt_max) {
                TWDR = memory[tx_buffer_cnt];
                tx_buffer_cnt++;
            } else {
                TWDR = 0;
            }
            break;

        case TW_ST_DATA_NACK:  // 0xC0
            break;

        default:
            // unexpected situation. should be checked!
            cli();
            hal::Serial0.print_string("TWI Slave error!\r\n");
            while (true) {
                hal::Serial0.print_byte(twsr);
            }
    }
    TWCR |= (1 << TWINT);
}

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
uint8_t ObcInterface<i2c_address, callback, rx_max_length, DataType>::rx_buffer[rx_max_length];

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
uint8_t ObcInterface<i2c_address, callback, rx_max_length, DataType>::rx_buffer_cnt = 0;

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
uint8_t *ObcInterface<i2c_address, callback, rx_max_length, DataType>::memory;

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
uint8_t ObcInterface<i2c_address, callback, rx_max_length, DataType>::tx_buffer_cnt = 0;

}  // namespace drivers
