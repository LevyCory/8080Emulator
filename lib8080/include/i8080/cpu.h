#pragma once

#include "asm.h"
#include "bus.h"

#include <limits>
#include <optional>

namespace i8080
{
class Cpu final
{
    struct Flags
    {
        union
        {
            uint8_t status;
            struct
            {
                uint8_t carry : 1;
                uint8_t reserved1 : 1;
                uint8_t parity : 1;
                uint8_t reserved2 : 1;
                uint8_t aux : 1;
                uint8_t reserved3 : 1;
                uint8_t zero : 1;
                uint8_t sign : 1;
            };
        };
    } __attribute__((packed));

#define DEFINE_REGISTER(high, low) \
    union                          \
    {                              \
        uint16_t high##low;        \
        struct                     \
        {                          \
            uint8_t low;           \
            uint8_t high;          \
        };                         \
    }

public:
    static constexpr uint16_t NAMESPACE_SIZE = std::numeric_limits<uint16_t>::max();

    struct State
    {
        union
        {
            uint16_t af;
            struct
            {
                uint8_t a;
                Flags flags;
            };
        };

        DEFINE_REGISTER(b, c); // bc
        DEFINE_REGISTER(d, e); // de
        DEFINE_REGISTER(h, l); // hl

        uint16_t pc;
        uint16_t sp;

        uint64_t cycle;

        bool halt;
        bool interrupts_enabled;

        std::optional<Instruction> interrupt_vector;
    };

#undef DEFINE_REGISTER

public:
    Cpu(Bus& bus, uint16_t entry_point);
    Cpu(const Cpu&) = delete;
    Cpu& operator=(const Cpu&) = delete;

    void set_debug(bool debug) { _debug = debug; }

    const State& state() const { return _state; }

    void tick();

    bool halt() const { return _state.halt; }

    void interrupt(Instruction instruction);
    void interrupt(uint8_t isr_number);

private:
    static constexpr uint8_t CONDITION_MET_CYCLE_COUNT = 6;

    static bool _get_parity(uint16_t number);
    const Opcode& _fetch() const;
    void _execute(const Opcode& opcode);

    static bool _is_zero(uint8_t number) { return number == 0; }

    static bool _is_signed(uint16_t number) { return (number & 0x80) != 0; }

    static bool _is_carry(uint16_t number) { return number > 0xff; }

    static bool _is_carry(uint32_t number) { return number > 0xffff; }

    static bool _is_carry(uint8_t number) { return _is_carry(static_cast<uint16_t>(number)); }

    // Instructions
    template <typename T>
    void _bitwise_instruction(uint8_t reg)
    {
        _state.a = T()(_state.a, reg);

        _set_zero_parity_sign(_state.a);

        _state.flags.carry = 0;
        _state.flags.aux = 0;
    }

    void _add(uint16_t value, uint8_t carry);
    void _ret_if(bool condition);
    void _jmp_if(bool condition, uint16_t address);
    void _call_if(bool condition, uint16_t address);
    void _set_zero_parity_sign(uint16_t value);
    void _set_zero_parity_sign(uint8_t value);

    // NOLINTBEGIN
    void _ADD(uint8_t reg);
    void _ADC(uint8_t reg);
    void _SUB(uint8_t reg);
    void _SBB(uint8_t reg);
    void _ANA(uint8_t reg);
    void _XRA(uint8_t reg);
    void _ORA(uint8_t reg);
    void _CMP(uint8_t reg);
    void _INR(uint8_t& reg);
    void _DCR(uint8_t& reg);
    void _DAD(uint16_t reg);
    void _PUSH(uint16_t reg);
    void _POP(uint16_t& reg);
    void _DAA();
    // NOLINTEND

    bool _debug;

    std::reference_wrapper<Bus> _bus;
    State _state;
};
} // namespace i8080
