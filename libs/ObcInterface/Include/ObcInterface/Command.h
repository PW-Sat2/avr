#ifndef LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMAND_H_
#define LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMAND_H_

#include <cstdint>

/*!
 * Command opcode and params definitions.
 * Every command should derive from it.
 */
template<std::uint8_t command_opcode, std::uint8_t command_params>
class Command {
 public:
    /*!
     * Command opcode (first byte in I2C write)
     */
    constexpr static std::uint8_t opcode = command_opcode;
    /*!
     * Requested number of parameters.
     */
    constexpr static std::uint8_t params = command_params;
};

#endif  // LIBS_OBCINTERFACE_INCLUDE_OBCINTERFACE_COMMAND_H_
