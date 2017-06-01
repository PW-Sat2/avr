#include <hal/hal>
#include <util/atomic.h>

namespace drivers {

/*!
 * Interface for communication with PW-Sat2 On-Board Computer.
 * This interface sets up TWI module as I2C slave.
 * Functionality:
 *   - send the commands (START+W, DATA..., STOP)
 *   - retrieve internal memory (START+W, ADDR, START+R, ERROR, DATA[ADDR], DATA[ADDR+1], ..., STOP)
 * This module assumes continuous memory layout of @tparam DataType.
 * First byte of @tparam DataType is 'error byte', which is retrieved as a first byte in every transaction.
 * Next bytes are addressed from '0'.
 * @tparam i2c_address 7-bit address on I2C bus
 * @tparam callback Callback which will be invoked after I2C STOP. Parameters will be data written to device.
 * @tparam cmd_max_parameters Maximum number of parameters to send to callback.
 * @tparam DataType Internal memory data type. First byte of this type is 'error byte'.
 * Following data are addressed from '0' during data readout.
 */
template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t cmd_max_parameters, typename DataType>
class ObcInterface {
 public:
    static_assert(sizeof(DataType) > 1, "DataType have to contain error code + at least one data field");
    static_assert(i2c_address <= 0x7F, "I2C address should be 7-bit!");

    /*!
     * Initialises TWI interface and sets up default memory location.
     * Additionally, enables pull-ups on TWI pins.
     * @param memory_ Default memory address to set a data retrieval pool
     */
    static void init(DataType *memory_) {
        set_memory(memory_);

        hal::DigitalIO::GPIO<hal::mcu::pin_sda>().init(hal::DigitalIO::Interface::Mode::INPUT_PULLUP);
        hal::DigitalIO::GPIO<hal::mcu::pin_scl>().init(hal::DigitalIO::Interface::Mode::INPUT_PULLUP);
        
        TWAR = (i2c_address << 1);
        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
    }

    /*!
     * Disables TWI slave interface.
     */
    static void disable() {
        TWCR = 0;
    }

    /*!
     * Overrides memory pointer to another location.
     * This action will take effect on next READ command.
     * Operation is atomic - turns off (if enabled) and restore interrupt configuration.
     * @param memory_ Pointer to new location.
     */
    static inline void set_memory(DataType *memory_) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            memory = reinterpret_cast<uint8_t *>(memory_);
        }
    }

    /*!
     * This method should be invoked from interrupt in calling code.
     */
    static inline void process_interrupt();
 private:

    static uint8_t rx_buffer[cmd_max_parameters];
    static uint8_t rx_buffer_cnt;
    static constexpr uint8_t rx_buffer_cnt_max = cmd_max_parameters;
    
    static uint8_t *memory;
    static uint8_t *memory_buffered;
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
            tx_buffer_cnt = rx_buffer[0]+1;
            memory_buffered = memory;
            TWDR = memory_buffered[0];
            break;
        
        case TW_ST_DATA_ACK:  // 0xB8
            if (tx_buffer_cnt < tx_buffer_cnt_max) {
                TWDR = memory_buffered[tx_buffer_cnt];
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
uint8_t *ObcInterface<i2c_address, callback, rx_max_length, DataType>::memory_buffered;

template<uint8_t i2c_address, void (*callback)(hal::libs::span<uint8_t>), uint8_t rx_max_length, typename DataType>
uint8_t ObcInterface<i2c_address, callback, rx_max_length, DataType>::tx_buffer_cnt = 0;

}  // namespace drivers
