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

struct GPGPU_WALKER {
    struct TheStructure {
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
    } Bitfield;
    enum Codes {
        DWORD_LENGTH_DWORD_COUNT_N = 0xd,
        SUBOPCODE_GPGPU_WALKER_SUBOP = 0x5,
        MEDIA_COMMAND_OPCODE_GPGPU_WALKER = 0x1,
        PIPELINE_MEDIA = 0x2,
        COMMAND_TYPE_GFXPIPE = 0x3,
        SIMD_SIZE_SIMD8 = 0x0,
        SIMD_SIZE_SIMD16 = 0x1,
        SIMD_SIZE_SIMD32 = 0x2,
        INDIRECTDATASTARTADDRESS_BYTEOFFSET = 0xc,
        INDIRECTDATASTARTADDRESS_INDEX = 0x3
    };
    static GPGPU_WALKER init() {
        GPGPU_WALKER state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield.Subopcode = SUBOPCODE_GPGPU_WALKER_SUBOP;
        state.Bitfield.MediaCommandOpcode = MEDIA_COMMAND_OPCODE_GPGPU_WALKER;
        state.Bitfield.Pipeline = PIPELINE_MEDIA;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        state.Bitfield.SimdSize = SIMD_SIZE_SIMD8;
        return state;
    }
};

struct PIPE_CONTROL {
    struct TheStructure {
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        uint32_t DepthCacheFlushEnable : BITFIELD_RANGE(0, 0);
        uint32_t StallAtPixelScoreboard : BITFIELD_RANGE(1, 1);
        uint32_t StateCacheInvalidationEnable : BITFIELD_RANGE(2, 2);
        uint32_t ConstantCacheInvalidationEnable : BITFIELD_RANGE(3, 3);
        uint32_t VfCacheInvalidationEnable : BITFIELD_RANGE(4, 4);
        uint32_t DcFlushEnable : BITFIELD_RANGE(5, 5);
        uint32_t ProtectedMemoryApplicationId : BITFIELD_RANGE(6, 6);
        uint32_t PipeControlFlushEnable : BITFIELD_RANGE(7, 7);
        uint32_t NotifyEnable : BITFIELD_RANGE(8, 8);
        uint32_t IndirectStatePointersDisable : BITFIELD_RANGE(9, 9);
        uint32_t TextureCacheInvalidationEnable : BITFIELD_RANGE(10, 10);
        uint32_t InstructionCacheInvalidateEnable : BITFIELD_RANGE(11, 11);
        uint32_t RenderTargetCacheFlushEnable : BITFIELD_RANGE(12, 12);
        uint32_t DepthStallEnable : BITFIELD_RANGE(13, 13);
        uint32_t PostSyncOperation : BITFIELD_RANGE(14, 15);
        uint32_t GenericMediaStateClear : BITFIELD_RANGE(16, 16);
        uint32_t Reserved_49 : BITFIELD_RANGE(17, 17);
        uint32_t TlbInvalidate : BITFIELD_RANGE(18, 18);
        uint32_t GlobalSnapshotCountReset : BITFIELD_RANGE(19, 19);
        uint32_t CommandStreamerStallEnable : BITFIELD_RANGE(20, 20);
        uint32_t StoreDataIndex : BITFIELD_RANGE(21, 21);
        uint32_t Reserved_54 : BITFIELD_RANGE(22, 22);
        uint32_t LriPostSyncOperation : BITFIELD_RANGE(23, 23);
        uint32_t DestinationAddressType : BITFIELD_RANGE(24, 24);
        uint32_t Reserved_57 : BITFIELD_RANGE(25, 25);
        uint32_t FlushLlc : BITFIELD_RANGE(26, 26);
        uint32_t ProtectedMemoryDisable : BITFIELD_RANGE(27, 27);
        uint32_t Reserved_60 : BITFIELD_RANGE(28, 31);
        uint32_t Reserved_64 : BITFIELD_RANGE(0, 1);
        uint32_t Address : BITFIELD_RANGE(2, 31);
        uint32_t AddressHigh;
        uint64_t ImmediateData;
    } Bitfield;
    enum {
        DWORD_LENGTH_DWORD_COUNT_N = 0x4,
        _3D_COMMAND_SUB_OPCODE_PIPE_CONTROL = 0x0,
        _3D_COMMAND_OPCODE_PIPE_CONTROL = 0x2,
        COMMAND_SUBTYPE_GFXPIPE_3D = 0x3,
        COMMAND_TYPE_GFXPIPE = 0x3,
        POST_SYNC_OPERATION_NO_WRITE = 0x0,
        POST_SYNC_OPERATION_WRITE_IMMEDIATE_DATA = 0x1,
        POST_SYNC_OPERATION_WRITE_PS_DEPTH_COUNT = 0x2,
        POST_SYNC_OPERATION_WRITE_TIMESTAMP = 0x3,
        GLOBAL_SNAPSHOT_COUNT_RESET_DON_T_RESET = 0x0,
        GLOBAL_SNAPSHOT_COUNT_RESET_RESET = 0x1,
        LRI_POST_SYNC_OPERATION_NO_LRI_OPERATION = 0x0,
        LRI_POST_SYNC_OPERATION_MMIO_WRITE_IMMEDIATE_DATA = 0x1,
        DESTINATION_ADDRESS_TYPE_PPGTT = 0x0,
        DESTINATION_ADDRESS_TYPE_GGTT = 0x1,
        ADDRESS_BYTEOFFSET = 0x8,
        ADDRESS_INDEX = 0x2,
        ADDRESSHIGH_BYTEOFFSET = 0xc,
        ADDRESSHIGH_INDEX = 0x3
    };
    static PIPE_CONTROL init() {
        PIPE_CONTROL state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_PIPE_CONTROL;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_PIPE_CONTROL;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_3D;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        state.Bitfield.PostSyncOperation = POST_SYNC_OPERATION_NO_WRITE;
        state.Bitfield.GlobalSnapshotCountReset = GLOBAL_SNAPSHOT_COUNT_RESET_DON_T_RESET;
        state.Bitfield.LriPostSyncOperation = LRI_POST_SYNC_OPERATION_NO_LRI_OPERATION;
        state.Bitfield.DestinationAddressType = DESTINATION_ADDRESS_TYPE_PPGTT;
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

struct BINDING_TABLE_STATE {
    uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
    uint32_t SurfaceStatePointer : BITFIELD_RANGE(6, 31);
};
#pragma pack()





