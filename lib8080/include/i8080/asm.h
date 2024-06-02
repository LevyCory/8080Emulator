#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace i8080
{
struct OpcodeMetadata
{
    OpcodeMetadata(const std::string& _name, uint8_t _size, uint8_t _cycles) :
        name(_name),
        size(_size),
        cycles(_cycles)
    {}

    const std::string name;
    const uint8_t size;
    const uint8_t cycles;
};

// NOLINTBEGIN
// clang-format off
enum class Instruction : uint8_t
{
    NOP = 0x00, LXI_B, STAX_B, INX_B, INR_B, DCR_B, MVI_B, RLC, NOP1,
    DAD_B, LDAX_B, DCX_B, INR_C, DCR_C, MVI_C, RRC, NOP2,
    LXI_D, STAX_D, INX_D, INR_D, DCR_D, MVI_D, RAL, NOP3,
    DAD_D, LDAX_D, DCX_D, INR_E, DCR_E, MVI_E, RAR, RIM,
    LXI_H, SHLD, INX_H, INR_H, DCR_H, MVI_H, DAA, NOP4,
    DAD_H, LHLD, DCX_H, INR_L, DCR_L, MVI_L, CMA, SIM,
    LXI_SP, STA, INX_SP, INR_M, DCR_M, MVI_M, STC, NOP5,
    DAD_SP, LDA, DCX_SP, INR_A, DCR_A, MVI_A, CMC, MOV_B_B,
    MOV_B_C, MOV_B_D, MOV_B_E, MOV_B_H, MOV_B_L, MOV_B_M, MOV_B_A, MOV_C_B,
    MOV_C_C, MOV_C_D, MOV_C_E, MOV_C_H, MOV_C_L, MOV_C_M, MOV_C_A, MOV_D_B,
    MOV_D_C, MOV_D_D, MOV_D_E, MOV_D_H, MOV_D_L, MOV_D_M, MOV_D_A, MOV_E_B,
    MOV_E_C, MOV_E_D, MOV_E_E, MOV_E_H, MOV_E_L, MOV_E_M, MOV_E_A, MOV_H_B,
    MOV_H_C, MOV_H_D, MOV_H_E, MOV_H_H, MOV_H_L, MOV_H_M, MOV_H_A, MOV_L_B,
    MOV_L_C, MOV_L_D, MOV_L_E, MOV_L_H, MOV_L_L, MOV_L_M, MOV_L_A, MOV_M_B,
    MOV_M_C, MOV_M_D, MOV_M_E, MOV_M_H, MOV_M_L, HLT, MOV_M_A, MOV_A_B,
    MOV_A_C, MOV_A_D, MOV_A_E, MOV_A_H, MOV_A_L, MOV_A_M, MOV_A_A, ADD_B,
    ADD_C, ADD_D, ADD_E, ADD_H, ADD_L, ADD_M, ADD_A, ADC_B,
    ADC_C, ADC_D, ADC_E, ADC_H, ADC_L, ADC_M, ADC_A, SUB_B,
    SUB_C, SUB_D, SUB_E, SUB_H, SUB_L, SUB_M, SUB_A, SBB_B,
    SBB_C, SBB_D, SBB_E, SBB_H, SBB_L, SBB_M, SBB_A, ANA_B,
    ANA_C, ANA_D, ANA_E, ANA_H, ANA_L, ANA_M, ANA_A, XRA_B,
    XRA_C, XRA_D, XRA_E, XRA_H, XRA_L, XRA_M, XRA_A, ORA_B,
    ORA_C, ORA_D, ORA_E, ORA_H, ORA_L, ORA_M, ORA_A, CMP_B,
    CMP_C, CMP_D, CMP_E, CMP_H, CMP_L, CMP_M, CMP_A, RNZ,
    POP_B, JNZ, JMP, CNZ, PUSH_B, ADI, RST_0, RZ,
    RET, JZ, NOP6, CZ, CALL, ACI, RST_1, RNC,
    POP_D, JNC, OUT, CNC, PUSH_D, SUI, RST_2, RC,
    NOP7, JC, IN, CC, NOP8, SBI, RST_3, RPO,
    POP_H, JPO, XTHL, CPO, PUSH_H, ANI, RST_4, RPE,
    PCHL, JPE, XCHG, CPE, NOP9, XRI, RST_5, RP,
    POP_PSW, JP, DI, CP, PUSH_PSW, ORI, RST_6, RM,
    SPHL, JM, EI, CM, NOP10, CPI, RST_7,
}; 
// NOLINTEND

static const std::array<OpcodeMetadata, 256> METADATA = {
    OpcodeMetadata{"NOP", 1, 4},
    {"LXI B,", 3, 10},
    {"STAX B", 1, 7},
    {"INX B", 1, 5},
    {"INR B", 1, 5},
    {"DCR B", 1, 5},
    {"MVI B,", 2, 7},
    {"RLC", 1, 4},
    {"NOP1", 1, 4},
    {"DAD B", 1, 10},
    {"LDAX B", 1, 7},
    {"DCX B", 1, 5},
    {"INR C", 1, 5},
    {"DCR C", 1, 5},
    {"MVI C,", 2, 7},
    {"RRC", 1, 4},
    {"NOP2", 1, 4},
    {"LXI D,", 3, 10},
    {"STAX D", 1, 7},
    {"INX D", 1, 5},
    {"INR D", 1, 5},
    {"DCR D", 1, 5},
    {"MVI D,", 2, 7},
    {"RAL", 1, 4},
    {"NOP3", 1, 4},
    {"DAD D", 1, 10},
    {"LDAX D", 1, 7},
    {"DCX D", 1, 5},
    {"INR E", 1, 5},
    {"DCR E", 1, 5},
    {"MVI E,", 2, 7},
    {"RAR", 1, 4},
    {"RIM", 1, 4},
    {"LXI H,", 3, 10},
    {"SHLD", 3, 16},
    {"INX H", 1, 5},
    {"INR H", 1, 5},
    {"DCR H", 1, 5},
    {"MVI H,", 2, 7},
    {"DAA", 1, 4},
    {"NOP4", 1, 4},
    {"DAD H", 1, 10},
    {"LHLD", 3, 16},
    {"DCX H", 1, 5},
    {"INR L", 1, 5},
    {"DCR L", 1, 5},
    {"MVI L,", 2, 7},
    {"CMA", 1, 4},
    {"SIM", 1, 4},
    {"LXI SP,", 3, 10},
    {"STA", 3, 13},
    {"INX SP", 1, 5},
    {"INR M", 1, 10},
    {"DCR M", 1, 10},
    {"MVI M,", 2, 10},
    {"STC", 1, 4},
    {"NOP5", 1, 4},
    {"DAD SP", 1, 10},
    {"LDA", 3, 13},
    {"DCX SP", 1, 5},
    {"INR A", 1, 5},
    {"DCR A", 1, 5},
    {"MVI A,", 2, 7},
    {"CMC", 1, 4},
    {"MOV B B", 1, 5},
    {"MOV B, C", 1, 5},
    {"MOV B, D", 1, 5},
    {"MOV B, E", 1, 5},
    {"MOV B, H", 1, 5},
    {"MOV B, L", 1, 5},
    {"MOV B, M", 1, 7},
    {"MOV B, A", 1, 5},
    {"MOV C, B", 1, 5},
    {"MOV C, C", 1, 5},
    {"MOV C, D", 1, 5},
    {"MOV C, E", 1, 5},
    {"MOV C, H", 1, 5},
    {"MOV C, L", 1, 5},
    {"MOV C, M", 1, 7},
    {"MOV C, A", 1, 5},
    {"MOV D, B", 1, 5},
    {"MOV D, C", 1, 5},
    {"MOV D, D", 1, 5},
    {"MOV D, E", 1, 5},
    {"MOV D, H", 1, 5},
    {"MOV D, L", 1, 5},
    {"MOV D, M", 1, 7},
    {"MOV D, A", 1, 5},
    {"MOV E, B", 1, 5},
    {"MOV E, C", 1, 5},
    {"MOV E, D", 1, 5},
    {"MOV E, E", 1, 5},
    {"MOV E, H", 1, 5},
    {"MOV E, L", 1, 5},
    {"MOV E, M", 1, 7},
    {"MOV E, A", 1, 5},
    {"MOV H, B", 1, 5},
    {"MOV H, C", 1, 5},
    {"MOV H, D", 1, 5},
    {"MOV H, E", 1, 5},
    {"MOV H, H", 1, 5},
    {"MOV H, L", 1, 5},
    {"MOV H, M", 1, 7},
    {"MOV H, A", 1, 5},
    {"MOV L, B", 1, 5},
    {"MOV L, C", 1, 5},
    {"MOV L, D", 1, 5},
    {"MOV L, E", 1, 5},
    {"MOV L, H", 1, 5},
    {"MOV L, L", 1, 5},
    {"MOV L, M", 1, 7},
    {"MOV L, A", 1, 5},
    {"MOV M, B", 1, 7},
    {"MOV M, C", 1, 7},
    {"MOV M, D", 1, 7},
    {"MOV M, E", 1, 7},
    {"MOV M, H", 1, 7},
    {"MOV M, L", 1, 7},
    {"HLT", 1, 7},
    {"MOV M, A", 1, 7},
    {"MOV A, B", 1, 5},
    {"MOV A, C", 1, 5},
    {"MOV A, D", 1, 5},
    {"MOV A, E", 1, 5},
    {"MOV A, H", 1, 5},
    {"MOV A, L", 1, 5},
    {"MOV A, M", 1, 7},
    {"MOV A, A", 1, 5},
    {"ADD B", 1, 4},
    {"ADD C", 1, 4},
    {"ADD D", 1, 4},
    {"ADD E", 1, 4},
    {"ADD H", 1, 4},
    {"ADD L", 1, 4},
    {"ADD M", 1, 7},
    {"ADD A", 1, 4},
    {"ADC B", 1, 4},
    {"ADC C", 1, 4},
    {"ADC D", 1, 4},
    {"ADC E", 1, 4},
    {"ADC H", 1, 4},
    {"ADC L", 1, 4},
    {"ADC M", 1, 7},
    {"ADC A", 1, 4},
    {"SUB B", 1, 4},
    {"SUB C", 1, 4},
    {"SUB D", 1, 4},
    {"SUB E", 1, 4},
    {"SUB H", 1, 4},
    {"SUB L", 1, 4},
    {"SUB M", 1, 7},
    {"SUB A", 1, 4},
    {"SBB B", 1, 4},
    {"SBB C", 1, 4},
    {"SBB D", 1, 4},
    {"SBB E", 1, 4},
    {"SBB H", 1, 4},
    {"SBB L", 1, 4},
    {"SBB M", 1, 7},
    {"SBB A", 1, 4},
    {"ANA B", 1, 4},
    {"ANA C", 1, 4},
    {"ANA D", 1, 4},
    {"ANA E", 1, 4},
    {"ANA H", 1, 4},
    {"ANA L", 1, 4},
    {"ANA M", 1, 7},
    {"ANA A", 1, 4},
    {"XRA B", 1, 4},
    {"XRA C", 1, 4},
    {"XRA D", 1, 4},
    {"XRA E", 1, 4},
    {"XRA H", 1, 4},
    {"XRA L", 1, 4},
    {"XRA M", 1, 7},
    {"XRA A", 1, 4},
    {"ORA B", 1, 4},
    {"ORA C", 1, 4},
    {"ORA D", 1, 4},
    {"ORA E", 1, 4},
    {"ORA H", 1, 4},
    {"ORA L", 1, 4},
    {"ORA M", 1, 7},
    {"ORA A", 1, 4},
    {"CMP B", 1, 4},
    {"CMP C", 1, 4},
    {"CMP D", 1, 4},
    {"CMP E", 1, 4},
    {"CMP H", 1, 4},
    {"CMP L", 1, 4},
    {"CMP M", 1, 7},
    {"CMP A", 1, 4},
    {"RNZ", 1, 5},
    {"POP B", 1, 10},
    {"JNZ", 3, 10},
    {"JMP", 3, 10},
    {"CNZ", 3, 11},
    {"PUSH B", 1, 11},
    {"ADI", 2, 7},
    {"RST 0", 1, 11},
    {"RZ", 1, 5},
    {"RET", 1, 10},
    {"JZ", 3, 10},
    {"NOP6", 1, 10},
    {"CZ", 3, 11},
    {"CALL", 3, 17},
    {"ACI", 2, 7},
    {"RST 1", 1, 11},
    {"RNC", 1, 5},
    {"POP D", 1, 10},
    {"JNC", 3, 10},
    {"OUT", 2, 10},
    {"CNC", 3, 11},
    {"PUSH D", 1, 11},
    {"SUI", 2, 7},
    {"RST 2", 1, 11},
    {"RC", 1, 5},
    {"NOP7", 1, 10},
    {"JC", 3, 10},
    {"IN", 2, 10},
    {"CC", 3, 11},
    {"NOP8", 1, 17},
    {"SBI", 2, 7},
    {"RST 3", 1, 11},
    {"RPO", 1, 5},
    {"POP H", 1, 10},
    {"JPO", 3, 10},
    {"XTHL", 1, 18},
    {"CPO", 3, 11},
    {"PUSH H", 1, 11},
    {"ANI", 2, 7},
    {"RST 4", 1, 11},
    {"RPE", 1, 5},
    {"PCHL", 1, 5},
    {"JPE", 3, 10},
    {"XCHG", 1, 4},
    {"CPE", 3, 11},
    {"NOP9", 1, 17},
    {"XRI", 2, 7},
    {"RST 5", 1, 11},
    {"RP", 1, 5},
    {"POP PSW", 1, 10},
    {"JP", 3, 10},
    {"DI", 1, 4},
    {"CP", 3, 11},
    {"PUSH PSW", 1, 11},
    {"ORI", 2, 7},
    {"RST 6", 1, 11},
    {"RM", 1, 5},
    {"SPHL", 1, 5},
    {"JM", 3, 10},
    {"EI", 1, 4},
    {"CM", 3, 11},
    {"NOP10", 1, 17},
    {"CPI", 2, 7},
    {"RST 7", 1, 11}
};
// clang-format off

struct Opcode
{
    Instruction instruction;
    union
    {
        uint16_t u16operand;
        uint8_t u8operand;
    };
} __attribute__((packed));

uint8_t isr_offset(Instruction instruction);
Instruction isr_to_rst(uint8_t isr_number);
void print_dissassembly(const Opcode& opcode, uint16_t pc);
const OpcodeMetadata& get_opcode_metadata(Instruction instruction);
}