#pragma once

#include <string.h>

#define BITFIELD_RANGE(startbit, endbit) ((endbit) - (startbit) + 1)

#pragma pack(1)
struct MEDIA_STATE_FLUSH {
    struct TheStructure {
        uint32_t DwordLength : BITFIELD_RANGE(0, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        uint32_t InterfaceDescriptorOffset : BITFIELD_RANGE(0, 5);
        uint32_t WatermarkRequired : BITFIELD_RANGE(6, 6);
        uint32_t FlushToGo : BITFIELD_RANGE(7, 7);
        uint32_t Reserved_40 : BITFIELD_RANGE(8, 31);
    } Bitfield;
    enum Codes {
        DWORD_LENGTH_DWORD_COUNT_N = 0x0,
        SUBOPCODE_MEDIA_STATE_FLUSH_SUBOP = 0x4,
        MEDIA_COMMAND_OPCODE_MEDIA_STATE_FLUSH = 0x0,
        PIPELINE_MEDIA = 0x2,
        COMMAND_TYPE_GFXPIPE = 0x3
    };
    static MEDIA_STATE_FLUSH init() {
        MEDIA_STATE_FLUSH state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield.Subopcode = SUBOPCODE_MEDIA_STATE_FLUSH_SUBOP;
        state.Bitfield.MediaCommandOpcode = MEDIA_COMMAND_OPCODE_MEDIA_STATE_FLUSH;
        state.Bitfield.Pipeline = PIPELINE_MEDIA;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};

struct MEDIA_INTERFACE_DESCRIPTOR_LOAD {
    struct TheStructure {
        uint32_t DwordLength : BITFIELD_RANGE(0, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        uint32_t Reserved_32;
        uint32_t InterfaceDescriptorTotalLength : BITFIELD_RANGE(0, 16);
        uint32_t Reserved_81 : BITFIELD_RANGE(17, 31);
        uint32_t InterfaceDescriptorDataStartAddress;
    } Bitfield;
    enum Codes {
        DWORD_LENGTH_DWORD_COUNT_N = 0x2,
        SUBOPCODE_MEDIA_INTERFACE_DESCRIPTOR_LOAD_SUBOP = 0x2,
        MEDIA_COMMAND_OPCODE_MEDIA_INTERFACE_DESCRIPTOR_LOAD = 0x0,
        PIPELINE_MEDIA = 0x2,
        COMMAND_TYPE_GFXPIPE = 0x3,
        INTERFACEDESCRIPTORDATASTARTADDRESS_BYTEOFFSET = 0xc,
        INTERFACEDESCRIPTORDATASTARTADDRESS_INDEX = 0x3
    };
    static MEDIA_INTERFACE_DESCRIPTOR_LOAD init() {
        MEDIA_INTERFACE_DESCRIPTOR_LOAD state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield.Subopcode = SUBOPCODE_MEDIA_INTERFACE_DESCRIPTOR_LOAD_SUBOP;
        state.Bitfield.MediaCommandOpcode = MEDIA_COMMAND_OPCODE_MEDIA_INTERFACE_DESCRIPTOR_LOAD;
        state.Bitfield.Pipeline = PIPELINE_MEDIA;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};

struct INTERFACE_DESCRIPTOR_DATA {
    uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
    uint32_t KernelStartPointer : BITFIELD_RANGE(6, 31);
    uint32_t KernelStartPointerHigh : BITFIELD_RANGE(0, 15);
    uint32_t Reserved_48 : BITFIELD_RANGE(16, 31);
    uint32_t Reserved_64 : BITFIELD_RANGE(0, 6);
    uint32_t SoftwareExceptionEnable : BITFIELD_RANGE(7, 7);
    uint32_t Reserved_72 : BITFIELD_RANGE(8, 10);
    uint32_t MaskStackExceptionEnable : BITFIELD_RANGE(11, 11);
    uint32_t Reserved_76 : BITFIELD_RANGE(12, 12);
    uint32_t IllegalOpcodeExceptionEnable : BITFIELD_RANGE(13, 13);
    uint32_t Reserved_78 : BITFIELD_RANGE(14, 15);
    uint32_t FloatingPointMode : BITFIELD_RANGE(16, 16);
    uint32_t ThreadPriority : BITFIELD_RANGE(17, 17);
    uint32_t SingleProgramFlow : BITFIELD_RANGE(18, 18);
    uint32_t DenormMode : BITFIELD_RANGE(19, 19);
    uint32_t Reserved_84 : BITFIELD_RANGE(20, 31);
    uint32_t Reserved_96 : BITFIELD_RANGE(0, 1);
    uint32_t SamplerCount : BITFIELD_RANGE(2, 4);
    uint32_t SamplerStatePointer : BITFIELD_RANGE(5, 31);
    uint32_t BindingTableEntryCount : BITFIELD_RANGE(0, 4);
    uint32_t BindingTablePointer : BITFIELD_RANGE(5, 15);
    uint32_t Reserved_144 : BITFIELD_RANGE(16, 31);
    uint32_t ConstantUrbEntryReadOffset : BITFIELD_RANGE(0, 15);
    uint32_t ConstantIndirectUrbEntryReadLength : BITFIELD_RANGE(16, 31);
    uint32_t NumberOfThreadsInGpgpuThreadGroup : BITFIELD_RANGE(0, 9);
    uint32_t Reserved_202 : BITFIELD_RANGE(10, 14);
    uint32_t GlobalBarrierEnable : BITFIELD_RANGE(15, 15);
    uint32_t SharedLocalMemorySize : BITFIELD_RANGE(16, 20);
    uint32_t BarrierEnable : BITFIELD_RANGE(21, 21);
    uint32_t RoundingMode : BITFIELD_RANGE(22, 23);
    uint32_t Reserved_216 : BITFIELD_RANGE(24, 31);
    uint32_t Cross_ThreadConstantDataReadLength : BITFIELD_RANGE(0, 7);
    uint32_t Reserved_232 : BITFIELD_RANGE(8, 31); 
};

struct GPGPU_WALKER {
    uint32_t DwordLength : BITFIELD_RANGE(0, 7);
    uint32_t PredicateEnable : BITFIELD_RANGE(8, 8);
    uint32_t Reserved_9 : BITFIELD_RANGE(9, 9);
    uint32_t IndirectParameterEnable : BITFIELD_RANGE(10, 10);
    uint32_t Reserved_11 : BITFIELD_RANGE(11, 15);
    uint32_t Subopcode : BITFIELD_RANGE(16, 23);
    uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
    uint32_t Pipeline : BITFIELD_RANGE(27, 28);
    uint32_t CommandType : BITFIELD_RANGE(29, 31);
    uint32_t InterfaceDescriptorOffset : BITFIELD_RANGE(0, 5);
    uint32_t Reserved_38 : BITFIELD_RANGE(6, 31);
    uint32_t IndirectDataLength : BITFIELD_RANGE(0, 16);
    uint32_t Reserved_81 : BITFIELD_RANGE(17, 31);
    uint32_t Reserved_96 : BITFIELD_RANGE(0, 5);
    uint32_t IndirectDataStartAddress : BITFIELD_RANGE(6, 31);
    uint32_t ThreadWidthCounterMaximum : BITFIELD_RANGE(0, 5);
    uint32_t Reserved_134 : BITFIELD_RANGE(6, 7);
    uint32_t ThreadHeightCounterMaximum : BITFIELD_RANGE(8, 13);
    uint32_t Reserved_142 : BITFIELD_RANGE(14, 15);
    uint32_t ThreadDepthCounterMaximum : BITFIELD_RANGE(16, 21);
    uint32_t Reserved_150 : BITFIELD_RANGE(22, 29);
    uint32_t SimdSize : BITFIELD_RANGE(30, 31);
    uint32_t ThreadGroupIdStartingX;
    uint32_t Reserved_192;
    uint32_t ThreadGroupIdXDimension;
    uint32_t ThreadGroupIdStartingY;
    uint32_t Reserved_288;
    uint32_t ThreadGroupIdYDimension;
    uint32_t ThreadGroupIdStartingResumeZ;
    uint32_t ThreadGroupIdZDimension;
    uint32_t RightExecutionMask;
    uint32_t BottomExecutionMask;
};

struct BINDING_TABLE_STATE {
    uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
    uint32_t SurfaceStatePointer : BITFIELD_RANGE(6, 31);
};
#pragma pack()





