#include "asm.h"

namespace i8080
{
    uint8_t cycle(Instruction instruction)
    {
        return CYCLES[static_cast<uint8_t>(instruction)];
    }

    uint8_t isr_offset(Instruction instruction)
    {
        return (static_cast<uint8_t>(instruction) - static_cast<uint8_t>(Instruction::RST_0));
    }

    Instruction isr_to_rst(uint8_t isr_number)
    {
        return static_cast<Instruction>(isr_number * 8 + static_cast<uint8_t>(Instruction::RST_0));
    }
}