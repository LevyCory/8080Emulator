#include "asm.h"

#include <fmt/core.h>

namespace i8080
{
enum class OperandType : uint8_t
{
    none = 1,
    byte = 2,
    word = 3
};

uint8_t isr_offset(Instruction instruction)
{
    return (static_cast<uint8_t>(instruction) - static_cast<uint8_t>(Instruction::RST_0));
}

Instruction isr_to_rst(uint8_t isr_number)
{
    return static_cast<Instruction>((isr_number * 8) + static_cast<uint8_t>(Instruction::RST_0));
}

void print_dissassembly(const Opcode& opcode, uint16_t pc)
{
    const OpcodeMetadata& metadata = get_opcode_metadata(opcode.instruction);
    fmt::print("{:#06x}    {}", pc, metadata.name);

    switch (static_cast<OperandType>(metadata.size)) {
    case OperandType::word:
        fmt::print(" {:#06x}", opcode.u16operand);
        break;
    case OperandType::byte:
        fmt::print(" {:#06x}", (opcode.u8operand & 0xff));
        break;
    default:
        break;
    }

    fmt::println("");
}

const OpcodeMetadata& get_opcode_metadata(Instruction instruction)
{
    return METADATA[static_cast<uint16_t>(instruction)];
}
} // namespace i8080
