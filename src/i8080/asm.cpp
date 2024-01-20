#include "asm.h"

#include <fmt/core.h>
#include <iostream>

namespace i8080
{
uint8_t isr_offset(Instruction instruction)
{
    return (static_cast<uint8_t>(instruction) - static_cast<uint8_t>(Instruction::RST_0));
}

Instruction isr_to_rst(uint8_t isr_number)
{
    return static_cast<Instruction>(isr_number * 8 + static_cast<uint8_t>(Instruction::RST_0));
}

void print_dissassembly(const Opcode& opcode, uint16_t pc)
{
    static constexpr uint8_t NO_OPERAND = 1;
    static constexpr uint8_t BYTE_OPERAND = 2;
    static constexpr uint8_t WORD_OPERAND = 3;

    const OpcodeMetadata& metadata = get_opcode_metadata(opcode.instruction);
    std::cout << fmt::format("{:#06x}    {}", pc, metadata.name);

    switch (metadata.size)
    {
    case WORD_OPERAND:
        std::cout << fmt::format(" {:#06x}", opcode.u16operand);
        break;
    case BYTE_OPERAND:
        std::cout << fmt::format(" {:#06x}", (opcode.u8operand & 0xff));
        break;
    default:
        break;
    }

    std::cout << std::endl;
}

const OpcodeMetadata& get_opcode_metadata(Instruction instruction)
{
    return METADATA[static_cast<uint16_t>(instruction)];
}
} // namespace i8080
