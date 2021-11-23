#include "asm.h"

#include <iostream>
#include <iomanip>

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

    void print_dissassembly(const Opcode& opcode)
    {
        std::cout << DISASSEMBLY[static_cast<uint8_t>(opcode.instruction)];

        switch (opcode.instruction)
        {
        case Instruction::JMP:
        case Instruction::JNZ:
        case Instruction::JNC:
        case Instruction::JPO:
        case Instruction::JP:
        case Instruction::JZ:
        case Instruction::JC:
        case Instruction::JPE:
        case Instruction::JM:
        case Instruction::CALL:
        case Instruction::CNZ:
        case Instruction::CPO:
        case Instruction::CPE:
        case Instruction::CZ:
        case Instruction::CC:
        case Instruction::CP:
        case Instruction::CM:
        case Instruction::LDA:
        case Instruction::STA:
        case Instruction::LHLD:
        case Instruction::SHLD:
        case Instruction::LXI_B:
        case Instruction::LXI_D:
        case Instruction::LXI_H:
        case Instruction::LXI_SP:
            //std::cout << " " << std::hex << opcode.operand << "h";
            std::cout << " " << std::hex << std::setfill('0') << std::setw(4) << opcode.u16operand << "h";
            break;

        case Instruction::IN:
        case Instruction::OUT:
        case Instruction::ORI:
        case Instruction::ANI:
        case Instruction::SUI:
        case Instruction::ADI:
        case Instruction::MVI_B:
        case Instruction::MVI_C:
        case Instruction::MVI_D:
        case Instruction::MVI_E:
        case Instruction::MVI_H:
        case Instruction::MVI_L:
        case Instruction::MVI_M:
        case Instruction::MVI_A:
            std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << opcode.u8operand << "h";
            break;

        default:
            break;
        }

        std::cout << "\n";
    }
}