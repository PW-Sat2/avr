#ifndef PLD_LIBS_PLD_COMMAND_H
#define PLD_LIBS_PLD_COMMAND_H

#include <ObcInterface/CommandDispatcher.h>
#include <hardware/interface.h>
#include <cstdint>

namespace pld {
namespace telecommands {

template<std::uint8_t opcode>
class PldCommand : public CommandWithOpcode<opcode, 0> {
 public:
    PldCommand(hardware::Interface*& hardware) : hardware(hardware) {
    }

    void invoke(gsl::span<const std::uint8_t> params) override {
        this->work(params);

        hardware->obc_interrupt_reset();
    }

    virtual void work(gsl::span<const std::uint8_t> params) = 0;

    hardware::Interface*& hardware;
};

}  // namespace telecommands
}  // namespace pld

#endif  // PLD_LIBS_PLD_COMMAND_H
