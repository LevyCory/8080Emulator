#pragma once

#include "common.h"
#include "asm.h"

#include <functional>

namespace i8080
{

class Cpu
{
public:
    Cpu(uint8_t* memory) :
        _memory(memory)
    {}

    void run(uint16_t code_section, uint16_t stack);

private:
    static bool _get_parity(uint16_t number);
    const Opcode& _fetch() const;
    bool _execute(const Opcode& opcode);

    template <typename T>
    bool _is_zero(T number) { return number == 0; }
    bool _is_signed(uint16_t number) { return (number & 0x80) != 0; }
    bool _is_carry(uint16_t number) { return number > 0xff; }
    bool _is_carry(uint32_t number) { return number > 0xffff; }
    bool _is_carry(uint8_t number) { return _is_carry(static_cast<uint16_t>(number)); }

    // Instructions
    template <typename T>
    void _bitwise_instruction(uint8_t reg)
    {
        _state.a = T()(_state.a, reg);
        _state.flags.zero = _is_zero(_state.a);
        _state.flags.sign = _is_carry(_state.a);
        _state.flags.parity = _get_parity(_state.a);

        _state.flags.carry = 0;
        _state.flags.aux = 0;
    }

    void _ret_if(bool condition);
    void _jmp_if(bool condition, uint16_t address);
    void _call_if(bool condition, uint16_t address);
    void _handle_change_by_1(uint8_t& reg, std::function<void(uint8_t&)> op);

    void _ADD(uint8_t reg);
    void _ADC(uint8_t reg);
    void _ANA(uint8_t reg);
    void _XRA(uint8_t reg);
    void _ORA(uint8_t reg);
    void _INR(uint8_t& reg);
    void _DCR(uint8_t& reg);
    void _LXI(uint16_t& reg);
    void _DAD(uint16_t reg);
    void _PUSH(uint16_t reg);
    void _POP(uint16_t& reg);
    void _RST(uint8_t offset);


#pragma pack(push, 1)
    struct Flags
    {
        union
        {
            uint8_t all;
            uint8_t carry:      1;
            uint8_t reserved1:  1;
            uint8_t parity:     1;
            uint8_t reserved2:  1;
            uint8_t aux:        1;
            uint8_t reserved3:  1;
            uint8_t zero:       1;
            uint8_t sign:       1;
        };
    };
#pragma pack(pop)

    struct State
    {
        union
        {
            uint16_t bc;
            struct
            {
                uint8_t c;
                uint8_t b;
            };
        };

        union
        {
            uint16_t af;
            struct
            {
                Flags flags;
                uint8_t a;
            };
        };

        union
        {
            uint16_t de;
            struct
            {
                uint8_t e;
                uint8_t d;
            };
        };

        union
        {
            uint16_t hl;
            struct
            {
                uint8_t l;
                uint8_t h;
            };
        };

        uint16_t pc;
        uint16_t sp;

        bool interrupts_enabled;
        uint8_t pending_interrupt;
    };


    State _state;
    uint8_t * const _memory;
};

}