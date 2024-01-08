#include "cpu.h"

#include <fmt/core.h>

#include <iostream>


namespace i8080
{
    Cpu::Cpu(Bus& bus) :
        _debug(false),
        _bus(bus)
    {
        _state.af = 0;
        _state.bc = 0;
        _state.de = 0;
        _state.hl = 0;
        _state.sp = 0;

        _state.pc = PROGRAM_START_OFFSET;

        _state.cycle = 0;
        _state.halt = false;
        _state.interrupts_enabled = true;
        _state.interrupt_vector = std::nullopt;
    }

    void Cpu::tick()
    {
         _execute(_bus.fetch(_state.pc)); 
    }

    void Cpu::interrupt(Instruction instruction)
    {
        // If interrupts are enabled, disable them and set the IV.
        // The ISR must re-enable the interrupts before returning
        if (_state.interrupts_enabled)
        {
            _state.interrupts_enabled = false;
            _state.interrupt_vector.emplace(instruction);
        }
    }

    void Cpu::interrupt(uint8_t isr_number)
    {
        interrupt(isr_to_rst(isr_number));
    }

    static void _s_increment(uint8_t& reg) { --reg; }
    static void _s_decrement(uint8_t& reg) { ++reg; }

    void Cpu::_handle_change_by_1(uint8_t& reg, std::function<void(uint8_t&)> op)
    {
        op(reg);
        _set_zero_parity_sign(reg);
        _state.flags.aux = (((_state.a & 0xf) + (reg & 0xf)) > 0xf);
    }

    void Cpu::_set_zero_parity_sign(uint8_t value)
    {
        _state.flags.zero = _is_zero(value);
        _state.flags.sign = _is_signed(value & 0x80);
        _state.flags.parity = _get_parity(value);
    }

    void Cpu::_set_zero_parity_sign(uint16_t value)
    {
        _state.flags.zero = _is_zero(value & 0xff);
        _state.flags.sign = _is_signed(value & 0x80);
        _state.flags.parity = _get_parity(value & 0xff);
    }

    void Cpu::_add(uint16_t value, uint8_t carry)
    {
        uint16_t result = static_cast<uint16_t>(_state.a) + (value + carry);

        _set_zero_parity_sign(result);

        _state.flags.carry = _is_carry(result);
        _state.flags.aux = (((_state.a & 0xf) + (value & 0xf) + carry) > 0xf);

        _state.a = result & 0xff;
    }

    void Cpu::_ret_if(bool condition)
    {
        if (condition)
        {
            _POP(_state.pc);
            _state.pc--;
            _state.cycle += CONDITION_MET_CYCLE_COUNT;
        }
    }

    void Cpu::_jmp_if(bool condition, uint16_t address)
    {
        if (condition)
        {
            // Subtract one to compensate for pc increment
            _state.pc = address - 1;
            return;
        }

        _state.pc += 2;
    }

    void Cpu::_call_if(bool condition, uint16_t address)
    {
        if (condition)
        {
            _PUSH(_state.pc + 3);
            _jmp_if(true, address);
            _state.cycle += CONDITION_MET_CYCLE_COUNT;
            return;
        }

        _state.pc += 2;
    }

    // Instructions
    void Cpu::_ADD(uint8_t reg)
    {
        _add(reg, 0);
    }

    void Cpu::_ADC(uint8_t reg)
    {
        _add(reg, _state.flags.carry);
    }

    void Cpu::_SUB(uint8_t reg)
    {
        _add(-reg, 0);
    }

    void Cpu::_SBB(uint8_t reg)
    {
        _add(-reg, -_state.flags.carry);
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

    void Cpu::_CMP(uint8_t reg)
    {
        int16_t result = _state.a - reg;
        _state.flags.carry = result >> 8;
        _set_zero_parity_sign(static_cast<uint16_t>(result));
    }

    void Cpu::_INR(uint8_t& reg)
    {
        _handle_change_by_1(reg, _s_increment);
    }

    void Cpu::_DCR(uint8_t& reg)
    {
        _handle_change_by_1(reg, _s_decrement);
    }

    void Cpu::_DAD(uint16_t reg)
    {
        uint32_t result = static_cast<uint32_t>(_state.a) + static_cast<uint32_t>(reg);
        _state.flags.carry = _is_carry(result);
        _state.hl = result & 0xffff;
    }

    void Cpu::_PUSH(uint16_t reg)
    {
        _state.sp -= 2;
        _bus.mem_write(_state.sp, reg);
    }

    void Cpu::_POP(uint16_t& reg)
    {
        _bus.mem_read(_state.sp, reg);
        _state.sp += 2;
    }

    bool Cpu::_get_parity(uint16_t number)
    {
        uint8_t one_bits = 0;
        for (int i = 0; i < 16; i++)
        {
            one_bits += ((number >> i) & 1);
        }

        return !(one_bits & 1);
    }

    void Cpu::_execute(const Opcode& opcode)
    {
        if (_debug)
        {
            print_dissassembly(opcode, _state.pc + PROGRAM_START_OFFSET);
        }

        switch (opcode.instruction)
        {
        case Instruction::OUT:
            _bus.write(opcode.u8operand, _state.c);
            _state.pc++;
            break;
        case Instruction::IN:
            _bus.read(opcode.u8operand, _state.c);
            _state.pc++;
            break;
        case Instruction::SHLD:
            _bus.mem_write(_state.hl, opcode.u16operand);
            _state.pc += 2;
            break;
        case Instruction::LHLD:
            _bus.mem_read(opcode.u16operand, _state.hl);
            _state.pc += 2;
            break;
        case Instruction::STA:
            _bus.mem_write(opcode.u16operand, _state.a);
            _state.pc += 2;
            break;
        case Instruction::LDA:
            _bus.mem_read(opcode.u16operand, _state.a);
            _state.pc += 2;
            break;
        case Instruction::STC:
            _state.flags.carry = 1;
            break;
        case Instruction::EI:
            _state.interrupts_enabled = true;
            break;
        case Instruction::DI:
            _state.interrupts_enabled = false;
            break;
        case Instruction::XCHG:
            std::swap(_state.de, _state.hl);
            break;
        case Instruction::XTHL:
            std::swap(_state.sp, _state.hl);
            break;
        case Instruction::PCHL:
            _state.pc = _state.hl;
            break;
        case Instruction::SPHL:
            _state.sp = _state.hl;
            break;
        case Instruction::RAL:
        {
            // Rotate A left with the carry flag
            uint8_t old_carry = _state.flags.carry;
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a << 1) | old_carry;
        }
        case Instruction::RAR:
        {
            // Rotate A left with the carry flag
            uint8_t old_carry = _state.flags.carry;
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a >> 1) | old_carry;
        }
        case Instruction::RRC:
            // Roll A right
            _state.flags.carry = _state.a & 1;
            _state.a = (_state.a >> 1) | (_state.flags.carry << 7);
            break;
        case Instruction::RLC:
            // Roll A left
            _state.flags.carry = _state.a >> 7;
            _state.a = (_state.a << 1) | _state.flags.carry;
            break;
        case Instruction::CMC:
            _state.flags.carry = ~_state.flags.carry;
            break;

        // MOV
        case Instruction::MOV_B_C:
            _state.b = _state.c;
            break;
        case Instruction::MOV_B_D:
            _state.b = _state.d;
            break;
        case Instruction::MOV_B_E:
            _state.b = _state.e;
            break;
        case Instruction::MOV_B_H:
            _state.b = _state.h;
            break;
        case Instruction::MOV_B_L:
            _state.b = _state.l;
            break;
        case Instruction::MOV_B_M:
            _bus.mem_read(_state.hl, _state.b);
            break;
        case Instruction::MOV_B_A:
            _state.b = _state.a;
            break;
        case Instruction::MOV_C_B:
            _state.c = _state.b;
            break;
        case Instruction::MOV_C_D:
            _state.c = _state.d;
            break;
        case Instruction::MOV_C_E:
            _state.c = _state.e;
            break;
        case Instruction::MOV_C_H:
            _state.c = _state.h;
            break;
        case Instruction::MOV_C_L:
            _state.c = _state.l;
            break;
        case Instruction::MOV_C_M:
            _bus.mem_read(_state.hl, _state.c);
            break;
        case Instruction::MOV_C_A:
            _state.c = _state.a;
            break;
        case Instruction::MOV_D_B:
            _state.d = _state.b;
            break;
        case Instruction::MOV_D_C:
            _state.d = _state.c;
            break;
        case Instruction::MOV_D_E:
            _state.d = _state.e;
            break;
        case Instruction::MOV_D_H:
            _state.d = _state.h;
            break;
        case Instruction::MOV_D_L:
            _state.d = _state.l;
            break;
        case Instruction::MOV_D_M:
            _bus.mem_read(_state.hl, _state.d);
            break;
        case Instruction::MOV_D_A:
            _state.d = _state.a;
            break;
        case Instruction::MOV_E_B:
            _state.e = _state.b;
            break;
        case Instruction::MOV_E_C:
            _state.e = _state.c;
            break;
        case Instruction::MOV_E_D:
            _state.e = _state.d;
            break;
        case Instruction::MOV_E_H:
            _state.e = _state.h;
            break;
        case Instruction::MOV_E_L:
            _state.e = _state.l;
            break;
        case Instruction::MOV_E_M:
            _bus.mem_read(_state.hl, _state.e);
            break;
        case Instruction::MOV_E_A:
            _state.e = _state.a;
            break;
        case Instruction::MOV_H_B:
            _state.e = _state.b;
            break;
        case Instruction::MOV_H_C:
            _state.e = _state.c;
            break;
        case Instruction::MOV_H_D:
            _state.h = _state.d;
            break;
        case Instruction::MOV_H_E:
            _state.h = _state.e;
            break;
        case Instruction::MOV_H_L:
            _state.h = _state.l;
            break;
        case Instruction::MOV_H_M:
            _bus.mem_read(_state.hl, _state.h);
            break;
        case Instruction::MOV_H_A:
            _state.h = _state.a;
            break;
        case Instruction::MOV_L_B:
            _state.l = _state.b;
            break;
        case Instruction::MOV_L_C:
            _state.l = _state.c;
            break;
        case Instruction::MOV_L_D:
            _state.l = _state.d;
            break;
        case Instruction::MOV_L_E:
            _state.l = _state.e;
            break;
        case Instruction::MOV_L_H:
            _state.l = _state.h;
            break;
        case Instruction::MOV_L_M:
            _bus.mem_read(_state.hl, _state.l);
            break;
        case Instruction::MOV_L_A:
            _state.l = _state.a;
            break;
        case Instruction::MOV_M_B:
            _bus.mem_write(_state.hl, _state.b);
            break;
        case Instruction::MOV_M_C:
            _bus.mem_write(_state.hl, _state.c);
            break;
        case Instruction::MOV_M_D:
            _bus.mem_write(_state.hl, _state.d);
            break;
        case Instruction::MOV_M_E:
            _bus.mem_write(_state.hl, _state.e);
            break;
        case Instruction::MOV_M_H:
            _bus.mem_write(_state.hl, _state.h);
            break;
        case Instruction::MOV_M_L:
            _bus.mem_write(_state.hl, _state.l);
            break;
        case Instruction::MOV_M_A:
            _bus.mem_write(_state.hl, _state.a);
            break;
        case Instruction::MOV_A_B:
            _state.a = _state.b;
            break;
        case Instruction::MOV_A_C:
            _state.a = _state.c;
            break;
        case Instruction::MOV_A_D:
            _state.a = _state.d;
            break;
        case Instruction::MOV_A_E:
            _state.a = _state.e;
            break;
        case Instruction::MOV_A_H:
            _state.a = _state.h;
            break;
        case Instruction::MOV_A_L:
            _state.a = _state.a;
            break;
        case Instruction::MOV_A_M:
            _bus.mem_read(_state.hl, _state.a);
            break;

        // LXI
        case Instruction::LXI_B:
            _state.bc = opcode.u16operand;
            break;
        case Instruction::LXI_D:
            _state.de = opcode.u16operand;
            break;
        case Instruction::LXI_H:
            _state.hl = opcode.u16operand;
            break;
        case Instruction::LXI_SP:
            _state.sp = opcode.u16operand;
            break;

        // INX
        case Instruction::INX_B:
            _state.bc++;
            break;
        case Instruction::INX_D:
            _state.de++;
            break;
        case Instruction::INX_H:
            _state.hl++;
            break;
        case Instruction::INX_SP:
            _state.sp--;
            break;

        // DCX
        case Instruction::DCX_B:
            _state.bc--;
            break;
        case Instruction::DCX_D:
            _state.de--;
            break;
        case Instruction::DCX_H:
            _state.hl--;
            break;
        case Instruction::DCX_SP:
            _state.sp++;
            break;

        // STAX
        case Instruction::STAX_B:
            _bus.mem_write(_state.bc, _state.a);
            break;
        case Instruction::STAX_D:
            _bus.mem_write(_state.de, _state.a);
            break;

        // LDAX
        case Instruction::LDAX_B:
            _bus.mem_read(_state.bc, _state.a);
            break;
        case Instruction::LDAX_D:
            _bus.mem_read(_state.de, _state.a);
            break;

        // DAD
        case Instruction::DAD_B:
            _DAD(_state.bc);
            break;
        case Instruction::DAD_D:
            _DAD(_state.de);
            break;
        case Instruction::DAD_H:
            _DAD(_state.hl);
            break;
        case Instruction::DAD_SP:
            _DAD(_state.sp);
            break;

        // INR
        case Instruction::INR_B:
            _INR(_state.b);
            break;
        case Instruction::INR_D:
            _INR(_state.d);
            break;
        case Instruction::INR_H:
            _INR(_state.h);
            break;
        case Instruction::INR_M:
            _INR(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::INR_C:
            _INR(_state.c);
            break;
        case Instruction::INR_E:
            _INR(_state.e);
            break;
        case Instruction::INR_L:
            _INR(_state.l);
            break;
        case Instruction::INR_A:
            _INR(_state.a);
            break;

        // DCR
        case Instruction::DCR_B:
            _DCR(_state.b);
            break;
        case Instruction::DCR_D:
            _DCR(_state.d);
            break;
        case Instruction::DCR_H:
            _DCR(_state.h);
            break;
        case Instruction::DCR_M:
            _INR(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::DCR_C:
            _DCR(_state.c);
            break;
        case Instruction::DCR_E:
            _DCR(_state.e);
            break;
        case Instruction::DCR_L:
            _DCR(_state.l);
            break;
        case Instruction::DCR_A:
            _DCR(_state.a);
            break;

        // MVI
        case Instruction::MVI_B:
            _state.b = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_C:
            _state.c = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_D:
            _state.d = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_E:
            _state.e = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_H:
            _state.h = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_L:
            _state.l = opcode.u8operand;
            _state.pc++;
            break;
        case Instruction::MVI_M:
            _bus.mem_write(_state.hl, opcode.u16operand);
            _state.pc++;
            break;
        case Instruction::MVI_A:
            _state.a = opcode.u8operand;
            _state.pc++;
            break;

        // ADD
        case Instruction::ADD_B:
            _ADD(_state.b);
            break;
        case Instruction::ADD_C:
            _ADD(_state.c);
            break;
        case Instruction::ADD_D:
            _ADD(_state.d);
            break;
        case Instruction::ADD_E:
            _ADD(_state.e);
            break;
        case Instruction::ADD_H:
            _ADD(_state.h);
            break;
        case Instruction::ADD_L:
            _ADD(_state.l);
            break;
        case Instruction::ADD_M:
            _ADD(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::ADD_A:
            _ADD(_state.a);
            break;
        case Instruction::ADI:
            _ADD(opcode.u8operand);
            _state.pc++;
            break;

        // ADC
        case Instruction::ADC_B:
            _ADC(_state.b);
            break;
        case Instruction::ADC_C:
            _ADC(_state.c);
            break;
        case Instruction::ADC_D:
            _ADC(_state.d);
            break;
        case Instruction::ADC_E:
            _ADC(_state.e);
            break;
        case Instruction::ADC_H:
            _ADC(_state.h);
            break;
        case Instruction::ADC_L:
            _ADC(_state.l);
            break;
        case Instruction::ADC_M:
            _ADC(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::ADC_A:
            _ADC(_state.a);
            break;
        case Instruction::ACI:
            _ADC(opcode.u8operand);
            _state.pc++;
            break;

        // SUB
        case Instruction::SUB_B:
            _SUB(_state.b);
            break;
        case Instruction::SUB_C:
            _SUB(_state.c);
            break;
        case Instruction::SUB_D:
            _SUB(_state.d);
            break;
        case Instruction::SUB_E:
            _SUB(_state.e);
            break;
        case Instruction::SUB_H:
            _SUB(_state.h);
            break;
        case Instruction::SUB_L:
            _SUB(_state.l);
            break;
        case Instruction::SUB_M:
            _SUB(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::SUB_A:
            _SUB(_state.a);
            break;
        case Instruction::SUI:
            _SUB(opcode.u8operand);
            _state.pc++;
            break;

        case Instruction::CMP_C:
            _CMP(_state.c);
            break;
        case Instruction::CMP_D:
            _CMP(_state.d);
            break;
        case Instruction::CMP_E:
            _CMP(_state.e);
            break;
        case Instruction::CMP_H:
            _CMP(_state.h);
            break;
        case Instruction::CMP_L:
            _CMP(_state.l);
            break;
        case Instruction::CMP_M:
            _CMP(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::CMP_A:
            _CMP(_state.a);
            break;
        case Instruction::CMP_B:
            _CMP(_state.b);
            break;
        case Instruction::CPI:
            _CMP(opcode.u8operand);
            _state.pc++;
            break;


        // SBB
        case Instruction::SBB_B:
            _SBB(_state.b);
            break;
        case Instruction::SBB_C:
            _SBB(_state.c);
            break;
        case Instruction::SBB_D:
            _SBB(_state.d);
            break;
        case Instruction::SBB_E:
            _SBB(_state.e);
            break;
        case Instruction::SBB_H:
            _SBB(_state.h);
            break;
        case Instruction::SBB_L:
            _SBB(_state.l);
            break;
        case Instruction::SBB_M:
            _SBB(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::SBB_A:
            _SBB(_state.a);
            break;
        case Instruction::SBI:
            _SBB(opcode.u8operand);
            _state.pc++;
            break;

        // ANA
        case Instruction::ANA_B:
            _ANA(_state.b);
            break;
        case Instruction::ANA_C:
            _ANA(_state.c);
            break;
        case Instruction::ANA_D:
            _ANA(_state.d);
            break;
        case Instruction::ANA_E:
            _ANA(_state.e);
            break;
        case Instruction::ANA_H:
            _ANA(_state.h);
            break;
        case Instruction::ANA_L:
            _ANA(_state.l);
            break;
        case Instruction::ANA_M:
            _ANA(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::ANA_A:
            _ANA(_state.a);
            break;
        case Instruction::ANI:
            _ANA(opcode.u8operand);
            _state.pc++;
            break;

        // XRA
        case Instruction::XRA_B:
            _XRA(_state.b);
            break;
        case Instruction::XRA_C:
            _XRA(_state.c);
            break;
        case Instruction::XRA_D:
            _XRA(_state.d);
            break;
        case Instruction::XRA_E:
            _XRA(_state.e);
            break;
        case Instruction::XRA_H:
            _XRA(_state.h);
            break;
        case Instruction::XRA_L:
            _XRA(_state.l);
            break;
        case Instruction::XRA_M:
            _XRA(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::XRA_A:
            _XRA(_state.a);
            break;
        case Instruction::XRI:
            _XRA(opcode.u8operand);
            _state.pc++;
            break;

        // ORA
        case Instruction::ORA_B:
            _ORA(_state.b);
            break;
        case Instruction::ORA_C:
            _ORA(_state.c);
            break;
        case Instruction::ORA_D:
            _ORA(_state.d);
            break;
        case Instruction::ORA_E:
            _ORA(_state.e);
            break;
        case Instruction::ORA_H:
            _ORA(_state.h);
            break;
        case Instruction::ORA_L:
            _ORA(_state.l);
            break;
        case Instruction::ORA_M:
            _ORA(_bus.mem_read_u8_ref(_state.hl));
            break;
        case Instruction::ORA_A:
            _ORA(_state.a);
            break;
        case Instruction::ORI:
            _ORA(opcode.u8operand);
            _state.pc++;
            break;

        // POP
        case Instruction::POP_B:
            _POP(_state.bc);
            break;
        case Instruction::POP_D:
            _POP(_state.de);
            break;
        case Instruction::POP_H:
            _POP(_state.hl);
            break;
        case Instruction::POP_PSW:
            _POP(_state.af);
            break;

        // PUSH
        case Instruction::PUSH_B:
            _PUSH(_state.bc);
            break;
        case Instruction::PUSH_D:
            _PUSH(_state.de);
            break;
        case Instruction::PUSH_H:
            _PUSH(_state.hl);
            break;
        case Instruction::PUSH_PSW:
            _PUSH(_state.af);
            break;

        // RET instructions
        case Instruction::RET:
            _ret_if(true);
            break;
        case Instruction::RNZ:
            _ret_if(!_state.flags.zero);
            break;
        case Instruction::RNC:
            _ret_if(!_state.flags.carry);
            break;
        case Instruction::RPO:
            _ret_if(!_state.flags.parity);
            break;
        case Instruction::RP:
            _ret_if(!_state.flags.sign);
            break;
        case Instruction::RZ:
            _ret_if(_state.flags.zero);
            break;
        case Instruction::RC:
            _ret_if(_state.flags.carry);
            break;
        case Instruction::RPE:
            _ret_if(_state.flags.parity);
            break;
        case Instruction::RM:
            _ret_if(_state.flags.sign);
            break;

        // JMP instructions
        case Instruction::JMP:
            _jmp_if(true, opcode.u16operand);
            break;
        case Instruction::JNZ:
            _jmp_if(!_state.flags.zero, opcode.u16operand);
            break;
        case Instruction::JNC:
            _jmp_if(!_state.flags.carry, opcode.u16operand);
            break;
        case Instruction::JPO:
            _jmp_if(!_state.flags.parity, opcode.u16operand);
            break;
        case Instruction::JP:
            _jmp_if(!_state.flags.sign, opcode.u16operand);
            break;
        case Instruction::JZ:
            _jmp_if(_state.flags.zero, opcode.u16operand);
            break;
        case Instruction::JC:
            _jmp_if(_state.flags.carry, opcode.u16operand);
            break;
        case Instruction::JPE:
            _jmp_if(_state.flags.parity, opcode.u16operand);
            break;
        case Instruction::JM:
            _jmp_if(_state.flags.sign, opcode.u16operand);
            break;

        // CALL instructions
        case Instruction::CALL:
            _call_if(true, opcode.u16operand);
            break;
        case Instruction::CNZ:
            _call_if(!_state.flags.zero, opcode.u16operand);
            break;
        case Instruction::CNC:
            _call_if(!_state.flags.carry, opcode.u16operand);
            break;
        case Instruction::CPO:
            _call_if(!_state.flags.parity, opcode.u16operand);
            break;
        case Instruction::CPE:
            _call_if(!_state.flags.sign, opcode.u16operand);
            break;
        case Instruction::CZ:
            _call_if(_state.flags.zero, opcode.u16operand);
            break;
        case Instruction::CC:
            _call_if(_state.flags.carry, opcode.u16operand);
            break;
        case Instruction::CP:
            _call_if(_state.flags.parity, opcode.u16operand);
            break;
        case Instruction::CM:
            _call_if(_state.flags.sign, opcode.u16operand);
            break;

        // RST
        case Instruction::RST_0:
        case Instruction::RST_1:
        case Instruction::RST_2:
        case Instruction::RST_3:
        case Instruction::RST_4:
        case Instruction::RST_5:
        case Instruction::RST_6:
        case Instruction::RST_7:
            _PUSH(_state.pc);
            _state.pc = isr_offset(opcode.instruction);
            break;

        case Instruction::MOV_A_A:
        case Instruction::MOV_B_B:
        case Instruction::MOV_C_C:
        case Instruction::MOV_D_D:
        case Instruction::MOV_E_E:
        case Instruction::MOV_H_H:
        case Instruction::MOV_L_L:
        case Instruction::NOP:
        case Instruction::NOP1:
        case Instruction::NOP2:
        case Instruction::NOP3:
        case Instruction::NOP4:
        case Instruction::NOP5:
        case Instruction::NOP6:
        case Instruction::NOP7:
        case Instruction::NOP8:
        case Instruction::NOP9:
        case Instruction::NOP10:
            break;

        case Instruction::DAA:
        case Instruction::CMA:
        default:
            std::cerr << fmt::format("Unimplemented instruction {}, Halting CPU operation",
                                     std::to_string(static_cast<uint8_t>(opcode.instruction)));

        case Instruction::HLT:
            _state.halt = true;
            return;
        }
    
        if (_state.interrupts_enabled && _state.interrupt_vector.has_value())
        {
            _execute({ .instruction = _state.interrupt_vector.value() });
            _state.interrupt_vector.reset();
        }
        else
        {
            _state.cycle += cycle(opcode.instruction);
            _state.pc++;
        }
    }
}
