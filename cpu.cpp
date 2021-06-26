#include "cpu.h"

#include <iostream>


namespace i8080
{
    static void _s_increment(uint8_t& reg) { --reg; }
    static void _s_decrement(uint8_t& reg) { ++reg; }

    void Cpu::_handle_change_by_1(uint8_t& reg, std::function<void(uint8_t&)> op)
    {
        op(reg);
        _state.flags.zero = _is_zero(reg & 0xff);
        _state.flags.sign = _is_signed(reg & 0x80);
        _state.flags.parity = _get_parity(reg & 0xff);
        _state.flags.aux = (((_state.a & 0xf) + (reg & 0xf)) > 0xf);
    }

    void Cpu::_ret_if(bool condition)
    {
        if (condition)
        {
            _state.pc = *reinterpret_cast<uint16_t*>(_memory + _state.sp);
            _state.sp += 2;
        }
    }

    void Cpu::_jmp_if(bool condition, uint16_t address)
    {
        if (condition)
        {
            _state.pc = address;
        }
        else
        {
            _state.pc += 2;
        }
    }

    void Cpu::_call_if(bool condition, uint16_t address)
    {
        if (condition)
        {
            _PUSH(_state.pc);
            _jmp_if(true, address);
        }
    }

    // Instructions
    void Cpu::_ADD(uint8_t reg)
    {
        uint16_t result = static_cast<uint16_t>(_state.a) + static_cast<uint16_t>(reg);

        _state.flags.zero = _is_zero(result & 0xff);
        _state.flags.sign = _is_signed(result & 0x80);
        _state.flags.parity = _get_parity(result & 0xff);

        _state.flags.carry = _is_carry(result);
        _state.flags.aux = (((_state.a & 0xf) + (reg & 0xf)) > 0xf);

        _state.a = result & 0xff;
    }

    void Cpu::_ADC(uint8_t reg)
    {
        uint16_t result = static_cast<uint16_t>(_state.a) +
                          static_cast<uint16_t>(reg) + _state.flags.carry;

        _state.flags.zero = _is_zero(result & 0xff);
        _state.flags.sign = _is_signed(result & 0x80);
        _state.flags.parity = _get_parity(result & 0xff);

        _state.flags.carry = _is_carry(result);
        _state.flags.aux = (((_state.a & 0xf) + (reg & 0xf) + _state.flags.carry) > 0xf);

        _state.a = result & 0xff;
    }

    void Cpu::_ANA(uint8_t reg)
    {
        _bitwise_instruction<std::bit_and<uint8_t>>(reg);
    }

    void Cpu::_XRA(uint8_t reg)
    {
        _bitwise_instruction<std::bit_xor<uint8_t>>(reg);
    }

    void Cpu::_ORA(uint8_t reg)
    {
        _bitwise_instruction<std::bit_or<uint8_t>>(reg);
    }

    void Cpu::_INR(uint8_t& reg)
    {
        _handle_change_by_1(reg, _s_increment);
    }

    void Cpu::_DCR(uint8_t& reg)
    {
        _handle_change_by_1(reg, _s_decrement);
    }

    void Cpu::_LXI(uint16_t& reg)
    {
        _state.bc = _memory[_state.pc];
        _state.pc += 2;
    }

    void Cpu::_DAD(uint16_t reg)
    {
        uint32_t result = static_cast<uint32_t>(_state.a) + static_cast<uint32_t>(reg);
        _state.flags.carry = _is_carry(result);
        _state.hl = result & 0xffff;
    }

    void Cpu::_PUSH(uint16_t reg)
    {
        *reinterpret_cast<uint16_t*>(padd(_memory, _state.sp)) = reg;
        _state.sp -= 2;
    }

    void Cpu::_POP(uint16_t& reg)
    {
        reg = *reinterpret_cast<uint16_t*>(padd(_memory, _state.sp));
        _state.sp += 2;
    }

    void Cpu::_RST(uint8_t offset)
    {
        _PUSH(_state.pc);
        _state.pc = offset;
    }

    bool Cpu::_get_parity(uint16_t number)
    {
        uint16_t temp = number ^ (number >> 1);
        temp = temp ^ (temp >> 2);
        temp = temp ^ (temp >> 4);
        temp = temp ^ (temp >> 8);

        return temp & 1;
    }

    void Cpu::run(uint16_t code_section, uint16_t stack)
    {
        _state.pc = code_section;
        _state.sp = stack;

        while (_execute(_fetch()))
        {
            if (_state.interrupts_enabled)
            {

            }
        }
    }

    const Opcode& Cpu::_fetch() const
    {
        return *reinterpret_cast<const Opcode*>(padd(_memory, _state.pc));
    }

    bool Cpu::_execute(const Opcode& opcode)
    {
        switch (opcode.instruction)
        {
        case Instructions::SHLD:
            _memory[_state.hl] = opcode.operand;
            _state.pc += 2;
            break;
        case Instructions::LHLD:
            _state.hl = _memory[opcode.operand];
            _state.pc += 2;
            break;
        case Instructions::STA:
            _memory[opcode.operand] = _state.a;
            _state.pc += 2;
            break;
        case Instructions::LDA:
            _state.a = _memory[opcode.operand];
            _state.pc += 2;
            break;
        case Instructions::STC:
            _state.flags.carry = 1;
            break;
        case Instructions::EI:
            _state.interrupts_enabled = true;
            break;
        case Instructions::DI:
            _state.interrupts_enabled = false;
            break;
        case Instructions::XCHG:
            std::swap(_state.de, _state.hl);
            break;
        case Instructions::XTHL:
            std::swap(_state.sp, _state.hl);
            break;
        case Instructions::PCHL:
            _state.pc = _state.hl;
            break;
        case Instructions::SPHL:
            _state.sp = _state.hl;
            break;
        case Instructions::RAL:
        {
            // Rotate A left with the carry flag
            bool old_carry = _state.flags.carry;
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a << 1) | old_carry;
        }
        case Instructions::RAR:
        {
            // Rotate A left with the carry flag
            bool old_carry = _state.flags.carry;
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a >> 1) | old_carry;
        }
        case Instructions::RRC:
            // Roll A right
            _state.flags.carry = _state.a & 1;
            _state.a = (_state.a >> 1) | (_state.flags.carry << 7);
            break;
        case Instructions::RLC:
            // Roll A left
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a << 1) | _state.flags.carry;
            break;

        // MOV
        case Instructions::MOV_B_C:
            _state.b = _state.c;
            break;
        case Instructions::MOV_B_D:
            _state.b = _state.d;
            break;
        case Instructions::MOV_B_E:
            _state.b = _state.e;
            break;
        case Instructions::MOV_B_H:
            _state.b = _state.h;
            break;
        case Instructions::MOV_B_L:
            _state.b = _state.l;
            break;
        case Instructions::MOV_B_M:
            _state.b = _memory[_state.hl];
            break;
        case Instructions::MOV_B_A:
            _state.b = _state.a;
            break;
        case Instructions::MOV_C_B:
            _state.c = _state.b;
            break;
        case Instructions::MOV_C_D:
            _state.c = _state.d;
            break;
        case Instructions::MOV_C_E:
            _state.c = _state.e;
            break;
        case Instructions::MOV_C_H:
            _state.c = _state.h;
            break;
        case Instructions::MOV_C_L:
            _state.c = _state.l;
            break;
        case Instructions::MOV_C_M:
            _state.c = _memory[_state.hl];
            break;
        case Instructions::MOV_C_A:
            _state.c = _state.a;
            break;
        case Instructions::MOV_D_B:
            _state.d = _state.b;
            break;
        case Instructions::MOV_D_C:
            _state.d = _state.c;
            break;
        case Instructions::MOV_D_E:
            _state.d = _state.e;
            break;
        case Instructions::MOV_D_H:
            _state.d = _state.h;
            break;
        case Instructions::MOV_D_L:
            _state.d = _state.l;
            break;
        case Instructions::MOV_D_M:
            _state.d = _memory[_state.hl];
            break;
        case Instructions::MOV_D_A:
            _state.d = _state.a;
            break;
        case Instructions::MOV_E_B:
            _state.e = _state.b;
            break;
        case Instructions::MOV_E_C:
            _state.e = _state.c;
            break;
        case Instructions::MOV_E_D:
            _state.e = _state.d;
            break;
        case Instructions::MOV_E_H:
            _state.e = _state.h;
            break;
        case Instructions::MOV_E_L:
            _state.e = _state.l;
            break;
        case Instructions::MOV_E_M:
            _state.e = _memory[_state.hl];
            break;
        case Instructions::MOV_E_A:
            _state.e = _state.a;
            break;
        case Instructions::MOV_H_B:
            _state.e = _state.b;
            break;
        case Instructions::MOV_H_C:
            _state.e = _state.c;
            break;
        case Instructions::MOV_H_D:
            _state.h = _state.d;
            break;
        case Instructions::MOV_H_E:
            _state.h = _state.e;
            break;
        case Instructions::MOV_H_L:
            _state.h = _state.l;
            break;
        case Instructions::MOV_H_M:
            _state.h = _memory[_state.hl];
            break;
        case Instructions::MOV_H_A:
            _state.h = _state.a;
            break;
        case Instructions::MOV_L_B:
            _state.l = _state.b;
            break;
        case Instructions::MOV_L_C:
            _state.l = _state.c;
            break;
        case Instructions::MOV_L_D:
            _state.l = _state.d;
            break;
        case Instructions::MOV_L_E:
            _state.l = _state.e;
            break;
        case Instructions::MOV_L_H:
            _state.l = _state.h;
            break;
        case Instructions::MOV_L_M:
            _state.l = _memory[_state.hl];
            break;
        case Instructions::MOV_L_A:
            _state.l = _state.a;
            break;
        case Instructions::MOV_M_B:
            _memory[_state.hl] = _state.b;
            break;
        case Instructions::MOV_M_C:
            _memory[_state.hl] = _state.c;
            break;
        case Instructions::MOV_M_D:
            _memory[_state.hl] = _state.d;
            break;
        case Instructions::MOV_M_E:
            _memory[_state.hl] = _state.e;
            break;
        case Instructions::MOV_M_H:
            _memory[_state.hl] = _state.h;
            break;
        case Instructions::MOV_M_L:
            _memory[_state.hl] = _state.l;
            break;
        case Instructions::MOV_M_A:
            _memory[_state.hl] = _state.a;
            break;
        case Instructions::MOV_A_B:
            _state.a = _state.b;
            break;
        case Instructions::MOV_A_C:
            _state.a = _state.c;
            break;
        case Instructions::MOV_A_D:
            _state.a = _state.d;
            break;
        case Instructions::MOV_A_E:
            _state.a = _state.e;
            break;
        case Instructions::MOV_A_H:
            _state.a = _state.h;
            break;
        case Instructions::MOV_A_L:
            _state.a = _state.a;
            break;
        case Instructions::MOV_A_M:
            _state.a = _memory[_state.hl];
            break;

        // LXI
        case Instructions::LXI_B:
            _LXI(_state.bc);
            break;
        case Instructions::LXI_D:
            _LXI(_state.de);
            break;
        case Instructions::LXI_H:
            _LXI(_state.hl);
            break;
        case Instructions::LXI_SP:
            _LXI(_state.sp);
            break;
             

        // INX
        case Instructions::INX_B:
            _state.bc++;
            break;
        case Instructions::INX_D:
            _state.de++;
            break;
        case Instructions::INX_H:
            _state.hl++;
            break;
        case Instructions::INX_SP:
            _state.sp--;
            break;

        // DCX
        case Instructions::DCX_B:
            _state.bc--;
            break;
        case Instructions::DCX_D:
            _state.de--;
            break;
        case Instructions::DCX_H:
            _state.hl--;
            break;
        case Instructions::DCX_SP:
            _state.sp++;
            break;

        // STAX
        case Instructions::STAX_B:
            _memory[_state.bc] = _state.a;
            break;
        case Instructions::STAX_D:
            _memory[_state.de] = _state.a;
            break;

        // LDAX
        case Instructions::LDAX_B:
            _state.a = _memory[_state.bc];
            break;
        case Instructions::LDAX_D:
            _state.a = _memory[_state.de];
            break;

        // DAD
        case Instructions::DAD_B:
            _DAD(_state.bc);
            break;
        case Instructions::DAD_D:
            _DAD(_state.de);
            break;
        case Instructions::DAD_H:
            _DAD(_state.hl);
            break;
        case Instructions::DAD_SP:
            _DAD(_state.sp);
            break;

        // INR
        case Instructions::INR_B:
            _INR(_state.b);
            break;
        case Instructions::INR_D:
            _INR(_state.d);
            break;
        case Instructions::INR_H:
            _INR(_state.h);
            break;
        case Instructions::INR_M:
            _INR(_memory[_state.hl]);
            break;
        case Instructions::INR_C:
            _INR(_state.c);
            break;
        case Instructions::INR_E:
            _INR(_state.e);
            break;
        case Instructions::INR_L:
            _INR(_state.l);
            break;
        case Instructions::INR_A:
            _INR(_state.a);
            break;

        // DCR
        case Instructions::DCR_B:
            _DCR(_state.b);
            break;
        case Instructions::DCR_D:
            _DCR(_state.d);
            break;
        case Instructions::DCR_H:
            _DCR(_state.h);
            break;
        case Instructions::DCR_M:
            _INR(_memory[_state.hl]);
            break;
        case Instructions::DCR_C:
            _DCR(_state.c);
            break;
        case Instructions::DCR_E:
            _DCR(_state.e);
            break;
        case Instructions::DCR_L:
            _DCR(_state.l);
            break;
        case Instructions::DCR_A:
            _DCR(_state.a);
            break;

        // MVI
        case Instructions::MVI_B:
            _state.b = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_C:
            _state.c = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_D:
            _state.d = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_E:
            _state.e = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_H:
            _state.h = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_L:
            _state.l = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_M:
            _memory[_state.hl] = opcode.operands[0];
            _state.sp++;
            break;
        case Instructions::MVI_A:
            _state.a = opcode.operands[0];
            _state.sp++;
            break;

        // ADD
        case Instructions::ADD_B:
            _ADD(_state.b);
            break;
        case Instructions::ADD_C:
            _ADD(_state.c);
            break;
        case Instructions::ADD_D:
            _ADD(_state.d);
            break;
        case Instructions::ADD_E:
            _ADD(_state.e);
            break;
        case Instructions::ADD_H:
            _ADD(_state.h);
            break;
        case Instructions::ADD_L:
            _ADD(_state.l);
            break;
        case Instructions::ADD_M:
            _ADD(_memory[_state.hl]);
            break;
        case Instructions::ADD_A:
            _ADD(_state.a);
            break;
        case Instructions::ADI:
            _ADD(opcode.operands[0]);
            _state.pc++;
            break;

        // ADC
        case Instructions::ADC_B:
            _ADC(_state.b);
            break;
        case Instructions::ADC_C:
            _ADC(_state.c);
            break;
        case Instructions::ADC_D:
            _ADC(_state.d);
            break;
        case Instructions::ADC_E:
            _ADC(_state.e);
            break;
        case Instructions::ADC_H:
            _ADC(_state.h);
            break;
        case Instructions::ADC_L:
            _ADC(_state.l);
            break;
        case Instructions::ADC_M:
            _ADC(_memory[_state.hl]);
            break;
        case Instructions::ADC_A:
            _ADC(_state.a);
            break;

        // SUB and CMP
        case Instructions::SUB_B:
        case Instructions::CMP_B:
            _ADD(-_state.b);
            break;
        case Instructions::SUB_C:
        case Instructions::CMP_C:
            _ADD(-_state.c);
            break;
        case Instructions::SUB_D:
        case Instructions::CMP_D:
            _ADD(-_state.d);
            break;
        case Instructions::SUB_E:
        case Instructions::CMP_E:
            _ADD(-_state.e);
            break;
        case Instructions::SUB_H:
        case Instructions::CMP_H:
            _ADD(-_state.h);
            break;
        case Instructions::SUB_L:
        case Instructions::CMP_L:
            _ADD(-_state.l);
            break;
        case Instructions::SUB_M:
        case Instructions::CMP_M:
            _ADD(-_memory[_state.hl]);
            break;
        case Instructions::SUB_A:
        case Instructions::CMP_A:
            _ADD(-_state.a);
            break;
        case Instructions::SUI:
            _ADD(-opcode.operands[0]);
            _state.pc++;
            break;

        // SBB
        case Instructions::SBB_B:
            _ADC(-_state.b);
            break;
        case Instructions::SBB_C:
            _ADC(-_state.c);
            break;
        case Instructions::SBB_D:
            _ADC(-_state.d);
            break;
        case Instructions::SBB_E:
            _ADC(-_state.e);
            break;
        case Instructions::SBB_H:
            _ADC(-_state.h);
            break;
        case Instructions::SBB_L:
            _ADC(-_state.l);
            break;
        case Instructions::SBB_M:
            _ADC(-_memory[_state.hl]);
            break;
        case Instructions::SBB_A:
            _ADC(-_state.a);
            break;

        // ANA
        case Instructions::ANA_B:
            _ANA(_state.b);
            break;
        case Instructions::ANA_C:
            _ANA(_state.c);
            break;
        case Instructions::ANA_D:
            _ANA(_state.d);
            break;
        case Instructions::ANA_E:
            _ANA(_state.e);
            break;
        case Instructions::ANA_H:
            _ANA(_state.h);
            break;
        case Instructions::ANA_L:
            _ANA(_state.l);
            break;
        case Instructions::ANA_M:
            _ANA(_memory[_state.hl]);
            break;
        case Instructions::ANA_A:
            _ANA(_state.a);
            break;
        case Instructions::ANI:
            _ANA(opcode.operands[0]);
            _state.pc++;
            break;

        // XRA
        case Instructions::XRA_B:
            _XRA(_state.b);
            break;
        case Instructions::XRA_C:
            _XRA(_state.c);
            break;
        case Instructions::XRA_D:
            _XRA(_state.d);
            break;
        case Instructions::XRA_E:
            _XRA(_state.e);
            break;
        case Instructions::XRA_H:
            _XRA(_state.h);
            break;
        case Instructions::XRA_L:
            _XRA(_state.l);
            break;
        case Instructions::XRA_M:
            _XRA(_memory[_state.hl]);
            break;
        case Instructions::XRA_A:
            _XRA(_state.a);
            break;

        // ORA
        case Instructions::ORA_B:
            _ORA(_state.b);
            break;
        case Instructions::ORA_C:
            _ORA(_state.c);
            break;
        case Instructions::ORA_D:
            _ORA(_state.d);
            break;
        case Instructions::ORA_E:
            _ORA(_state.e);
            break;
        case Instructions::ORA_H:
            _ORA(_state.h);
            break;
        case Instructions::ORA_L:
            _ORA(_state.l);
            break;
        case Instructions::ORA_M:
            _ORA(_memory[_state.hl]);
            break;
        case Instructions::ORA_A:
            _ORA(_state.a);
            break;
        case Instructions::ORI:
            _ORA(opcode.operands[0]);
            _state.sp++;
            break;

        // POP
        case Instructions::POP_B:
            _POP(_state.bc);
            break;
        case Instructions::POP_D:
            _POP(_state.de);
            break;
        case Instructions::POP_H:
            _POP(_state.hl);
            break;
        case Instructions::POP_PSW:
            _POP(_state.af);
            break;

        // PUSH
        case Instructions::PUSH_B:
            _PUSH(_state.bc);
            break;
        case Instructions::PUSH_D:
            _PUSH(_state.de);
            break;
        case Instructions::PUSH_H:
            _PUSH(_state.hl);
            break;
        case Instructions::PUSH_PSW:
            _PUSH(_state.af);
            break;

        // RET instructions
        case Instructions::RET:
            _ret_if(true);
            break;
        case Instructions::RNZ:
            _ret_if(!_state.flags.zero);
            break;
        case Instructions::RNC:
            _ret_if(!_state.flags.carry);
            break;
        case Instructions::RPO:
            _ret_if(!_state.flags.parity);
            break;
        case Instructions::RP:
            _ret_if(!_state.flags.sign);
            break;
        case Instructions::RZ:
            _ret_if(_state.flags.zero);
            break;
        case Instructions::RC:
            _ret_if(_state.flags.carry);
            break;
        case Instructions::RPE:
            _ret_if(_state.flags.parity);
            break;
        case Instructions::RM:
            _ret_if(_state.flags.sign);
            break;


        // JMP instructions
        case Instructions::JMP:
            _jmp_if(true, opcode.operand);
            break;
        case Instructions::JNZ:
            _jmp_if(!_state.flags.zero, opcode.operand);
            break;
        case Instructions::JNC:
            _jmp_if(!_state.flags.carry, opcode.operand);
            break;
        case Instructions::JPO:
            _jmp_if(!_state.flags.parity, opcode.operand);
            break;
        case Instructions::JP:
            _jmp_if(!_state.flags.sign, opcode.operand);
            break;
        case Instructions::JZ:
            _jmp_if(_state.flags.zero, opcode.operand);
            break;
        case Instructions::JC:
            _jmp_if(_state.flags.carry, opcode.operand);
            break;
        case Instructions::JPE:
            _jmp_if(_state.flags.parity, opcode.operand);
            break;
        case Instructions::JM:
            _jmp_if(_state.flags.sign, opcode.operand);
            break;

        // CALL instructions
        case Instructions::CALL:
            _call_if(true, opcode.operand);
            break;
        case Instructions::CNZ:
            _call_if(!_state.flags.zero, opcode.operand);
            break;
        case Instructions::CPO:
            _call_if(!_state.flags.parity, opcode.operand);
            break;
        case Instructions::CPE:
            _call_if(!_state.flags.sign, opcode.operand);
            break;
        case Instructions::CZ:
            _call_if(_state.flags.zero, opcode.operand);
            break;
        case Instructions::CC:
            _call_if(_state.flags.carry, opcode.operand);
            break;
        case Instructions::CP:
            _call_if(_state.flags.parity, opcode.operand);
            break;
        case Instructions::CM:
            _call_if(!_state.flags.sign, opcode.operand);
            break;

        // RST
        case Instructions::RST_0:
            _RST(0x00);
            break;
        case Instructions::RST_1:
            _RST(0x08);
            break;
        case Instructions::RST_2:
            _RST(0x10);
            break;
        case Instructions::RST_3:
            _RST(0x18);
            break;
        case Instructions::RST_4:
            _RST(0x20);
            break;
        case Instructions::RST_5:
            _RST(0x28);
            break;
        case Instructions::RST_6:
            _RST(0x30);
            break;
        case Instructions::RST_7:
            _RST(0x38);
            break;

        // A MOV between a register and itself has a different cycle count than a NOP instruction
        case Instructions::MOV_A_A:
        case Instructions::MOV_B_B:
        case Instructions::MOV_C_C:
        case Instructions::MOV_D_D:
        case Instructions::MOV_E_E:
        case Instructions::MOV_H_H:
        case Instructions::MOV_L_L:
            break;

        case Instructions::NOP:
        case Instructions::NOP1:
        case Instructions::NOP2:
        case Instructions::NOP3:
        case Instructions::NOP4:
        case Instructions::NOP5:
        case Instructions::NOP6:
        case Instructions::NOP7:
        case Instructions::NOP8:
        case Instructions::NOP9:
        case Instructions::NOP10:
            break;

        default:
            std::cout << "Unimplemented instruction. Halting CPU operation.\n";
        case Instructions::HLT:
            return false;
        }

        _state.pc++;
        return true;
    }
}
