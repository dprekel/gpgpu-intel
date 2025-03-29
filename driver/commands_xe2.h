#pragma once

#include <string.h>

#define BITFIELD_RANGE(startbit, endbit) ((endbit) - (startbit) + 1)

#pragma pack(1)


// No MEDIA_STATE_FLUSH

struct STATE_BASE_ADDRESS {
    struct TheStructure {
        // DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        // DW1+DW2
        uint64_t GeneralStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_33 : BITFIELD_RANGE(1, 3);
        uint64_t GeneralStateMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t GeneralStateMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_43 : BITFIELD_RANGE(11, 11);
        uint64_t GeneralStateBaseAddress : BITFIELD_RANGE(12, 63);
        // DW3
        uint32_t Reserved_96 : BITFIELD_RANGE(0, 15);
        uint32_t StatelessDataPortAccessMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(16, 16);
        uint32_t StatelessDataPortAccessMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(17, 22);
        uint32_t L1CacheControlCachePolicy : BITFIELD_RANGE(23, 25);
        uint32_t Reserved_122 : BITFIELD_RANGE(26, 31);
        // DW4+DW5
        uint64_t SurfaceStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_129 : BITFIELD_RANGE(1, 3);
        uint64_t SurfaceStateMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t SurfaceStateMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_139 : BITFIELD_RANGE(11, 11);
        uint64_t SurfaceStateBaseAddress : BITFIELD_RANGE(12, 63);
        // DW6+DW7
        uint64_t DynamicStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_193 : BITFIELD_RANGE(1, 3);
        uint64_t DynamicStateMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t DynamicStateMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_203 : BITFIELD_RANGE(11, 11);
        uint64_t DynamicStateBaseAddress : BITFIELD_RANGE(12, 63);
        // DW8+DW9
        uint64_t Reserved_256;
        // DW10+DW11
        uint64_t InstructionBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_321 : BITFIELD_RANGE(1, 3);
        uint64_t InstructionMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t InstructionMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_331 : BITFIELD_RANGE(11, 11);
        uint64_t InstructionBaseAddress : BITFIELD_RANGE(12, 63);
        // DW12
        uint32_t GeneralStateBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_385 : BITFIELD_RANGE(1, 11);
        uint32_t GeneralStateBufferSize : BITFIELD_RANGE(12, 31);
        // DW13
        uint32_t DynamicStateBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_417 : BITFIELD_RANGE(1, 11);
        uint32_t DynamicStateBufferSize : BITFIELD_RANGE(12, 31);
        // DW14
        uint32_t Reserved_448;
        // DW15
        uint32_t InstructionBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_481 : BITFIELD_RANGE(1, 11);
        uint32_t InstructionBufferSize : BITFIELD_RANGE(12, 31);
        // DW16+DW17
        uint64_t BindlessSurfaceStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_513 : BITFIELD_RANGE(1, 3);
        uint64_t BindlessSurfaceStateMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t BindlessSurfaceStateMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_523 : BITFIELD_RANGE(11, 11);
        uint64_t BindlessSurfaceStateBaseAddress : BITFIELD_RANGE(12, 63);
        // DW18
        uint32_t BindlessSurfaceStateSize;
        // DW19+DW20
        uint64_t BindlessSamplerStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_609 : BITFIELD_RANGE(1, 3);
        uint64_t BindlessSamplerStateMemoryObjectControlStateEncryptedData : BITFIELD_RANGE(4, 4);
        uint64_t BindlessSamplerStateMemoryObjectControlStateIndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_619 : BITFIELD_RANGE(11, 11);
        uint64_t BindlessSamplerStateBaseAddress : BITFIELD_RANGE(12, 63);
        // DW21
        uint32_t Reserved_672 : BITFIELD_RANGE(0, 11);
        uint32_t BindlessSamplerStateBufferSize : BITFIELD_RANGE(12, 31);
    } Bitfield;
    enum {
        DWORD_LENGTH_DWORD_COUNT_N = 0x14,
        _3D_COMMAND_SUB_OPCODE_STATE_BASE_ADDRESS = 0x1,
        _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED = 0x1,
        COMMAND_SUBTYPE_GFXPIPE_COMMON = 0x0,
        COMMAND_TYPE_GFXPIPE = 0x3,
        L1_CACHE_CONTROL_WBP = 0x4
    };
    static STATE_BASE_ADDRESS init() {
        STATE_BASE_ADDRESS state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_STATE_BASE_ADDRESS;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_COMMON;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        state.Bitfield.L1CacheControlCachePolicy = L1_CACHE_CONTROL_WBP;
        return state;
    }
};

struct STATE_SIP {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1+DW2
        uint64_t Reserved_32 : BITFIELD_RANGE(0, 3);
        uint64_t SystemInstructionPointer : BITFIELD_RANGE(4, 63);
    } Bitfield;
    enum {
        DWORD_LENGTH_DWORD_COUNT_N = 0x1,
        _3D_COMMAND_SUB_OPCODE_STATE_SIP = 0x2,
        _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED = 0x1,
        COMMAND_SUBTYPE_GFXPIPE_COMMON = 0x0,
        COMMAND_TYPE_GFXPIPE = 0x3
    };
    static STATE_SIP init() {
        STATE_SIP state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_STATE_SIP;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_COMMON;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};



