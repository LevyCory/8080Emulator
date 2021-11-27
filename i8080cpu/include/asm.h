#pragma once

#include <cstdint>
#include <string_view>

namespace i8080
{
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

    static const char* DISASSEMBLY[] =
    {
        "NOP", "LXI B,", "STAX B", "INX B", "INR B", "DCR B", "MVI B,", "RLC", "NOP1",
        "DAD B", "LDAX B", "DCX B", "INR C", "DCR C", "MVI C,", "RRC", "NOP2",
        "LXI D,", "STAX D", "INX D", "INR D", "DCR D", "MVI D,", "RAL", "NOP3",
        "DAD D", "LDAX D", "DCX D", "INR E", "DCR E", "MVI E,", "RAR", "RIM",
        "LXI H,", "SHLD", "INX H", "INR H", "DCR H", "MVI H,", "DAA", "NOP4",
        "DAD H", "LHLD", "DCX H", "INR L", "DCR L", "MVI L,", "CMA", "SIM",
        "LXI SP,", "STA", "INX SP", "INR M", "DCR M", "MVI M,", "STC", "NOP5",
        "DAD SP", "LDA", "DCX SP", "INR A", "DCR A", "MVI A,", "CMC", "MOV B B",
        "MOV B, C", "MOV B, D", "MOV B, E", "MOV B, H", "MOV B, L", "MOV B, M", "MOV B, A", "MOV C, B",
        "MOV C, C", "MOV C, D", "MOV C, E", "MOV C, H", "MOV C, L", "MOV C, M", "MOV C, A", "MOV D, B",
        "MOV D, C", "MOV D, D", "MOV D, E", "MOV D, H", "MOV D, L", "MOV D, M", "MOV D, A", "MOV E, B",
        "MOV E, C", "MOV E, D", "MOV E, E", "MOV E, H", "MOV E, L", "MOV E, M", "MOV E, A", "MOV H, B",
        "MOV H, C", "MOV H, D", "MOV H, E", "MOV H, H", "MOV H, L", "MOV H, M", "MOV H, A", "MOV L, B",
        "MOV L, C", "MOV L, D", "MOV L, E", "MOV L, H", "MOV L, L", "MOV L, M", "MOV L, A", "MOV M, B",
        "MOV M, C", "MOV M, D", "MOV M, E", "MOV M, H", "MOV M, L", "HLT", "MOV M, A", "MOV A, B",
        "MOV A, C", "MOV A, D", "MOV A, E", "MOV A, H", "MOV A, L", "MOV A, M", "MOV A, A", "ADD B",
        "ADD C", "ADD D", "ADD E", "ADD H", "ADD L", "ADD M", "ADD A", "ADC B",
        "ADC C", "ADC D", "ADC E", "ADC H", "ADC L", "ADC M", "ADC A", "SUB B",
        "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB M", "SUB A", "SBB B",
        "SBB C", "SBB D", "SBB E", "SBB H", "SBB L", "SBB M", "SBB A", "ANA B",
        "ANA C", "ANA D", "ANA E", "ANA H", "ANA L", "ANA M", "ANA A", "XRA B",
        "XRA C", "XRA D", "XRA E", "XRA H", "XRA L", "XRA M", "XRA A", "ORA B",
        "ORA C", "ORA D", "ORA E", "ORA H", "ORA L", "ORA M", "ORA A", "CMP B",
        "CMP C", "CMP D", "CMP E", "CMP H", "CMP L", "CMP M", "CMP A", "RNZ",
        "POP B", "JNZ", "JMP", "CNZ", "PUSH B", "ADI", "RST 0", "RZ",
        "RET", "JZ", "NOP6", "CZ", "CALL", "ACI", "RST 1", "RNC",
        "POP D", "JNC", "OUT", "CNC", "PUSH D", "SUI", "RST 2", "RC",
        "NOP7", "JC", "IN", "CC", "NOP8", "SBI", "RST 3", "RPO",
        "POP H", "JPO", "XTHL", "CPO", "PUSH H", "ANI", "RST 4", "RPE",
        "PCHL", "JPE", "XCHG", "CPE", "NOP9", "XRI", "RST 5", "RP",
        "POP PSW", "JP", "DI", "CP", "PUSH PSW", "ORI", "RST 6", "RM",
        "SPHL", "JM", "EI", "CM", "NOP10", "CPI", "RST 7",
    };

    static constexpr uint8_t CYCLES[256] = {
        4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,
        4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,
        4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,
        4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,
        5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,
        5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,
        5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,
        7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,
        4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,
        4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,
        4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,
        4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,
        5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11,
        5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11,
        5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11,
        5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11
    };

    

#pragma pack(push, 1)
    struct Opcode
    {
        Instruction instruction;
        union
        {
            uint16_t u16operand;
            uint8_t u8operand;
        };
    };
#pragma pack(pop)

    uint8_t cycle(Instruction instruction);
    uint8_t isr_offset(Instruction instruction);
    Instruction isr_to_rst(uint8_t isr_number);
    void print_dissassembly(const Opcode& opcode);
}
