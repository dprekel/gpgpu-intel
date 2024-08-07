#pragma once

#include <string.h>

#define BITFIELD_RANGE(startbit, endbit) ((endbit) - (startbit) + 1)

#pragma pack(1)


//GEN11 too
struct MEDIA_STATE_FLUSH {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
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

// GEN11 too
struct MEDIA_INTERFACE_DESCRIPTOR_LOAD {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
        uint32_t Reserved_32;
        //DW2
        uint32_t InterfaceDescriptorTotalLength : BITFIELD_RANGE(0, 16);
        uint32_t Reserved_81 : BITFIELD_RANGE(17, 31);
        //DW3
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

// Some differences to GEN11
struct MEDIA_VFE_STATE {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
        uint32_t PerThreadScratchSpace : BITFIELD_RANGE(0, 3);
        uint32_t StackSize : BITFIELD_RANGE(4, 7);
        uint32_t Reserved_40 : BITFIELD_RANGE(8, 9);
        uint32_t ScratchSpaceBasePointer : BITFIELD_RANGE(10, 31);
        //DW2
        uint32_t ScratchSpaceBasePointerHigh : BITFIELD_RANGE(0, 15);
        uint32_t Reserved_80 : BITFIELD_RANGE(16, 31);
        //DW3
        uint32_t Reserved_96 : BITFIELD_RANGE(0, 6);
        uint32_t ResetGatewayTimer : BITFIELD_RANGE(7, 7);
        uint32_t NumberOfUrbEntries : BITFIELD_RANGE(8, 15);
        uint32_t MaximumNumberOfThreads : BITFIELD_RANGE(16, 31);
        //DW4
        uint32_t SliceDisable : BITFIELD_RANGE(0, 1);
        uint32_t Reserved_130 : BITFIELD_RANGE(2, 31);
        //DW5
        uint32_t CurbeAllocationSize : BITFIELD_RANGE(0, 15);
        uint32_t UrbEntryAllocationSize : BITFIELD_RANGE(16, 31);
        //DW6
        uint32_t ScoreboardMask : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_200 : BITFIELD_RANGE(8, 29);
        uint32_t ScoreboardType : BITFIELD_RANGE(30, 30);
        uint32_t ScoreboardEnable : BITFIELD_RANGE(31, 31);
        //DW7
        uint32_t Scoreboard0DeltaX : BITFIELD_RANGE(0, 3);
        uint32_t Scoreboard0DeltaY : BITFIELD_RANGE(4, 7);
        uint32_t Scoreboard1DeltaX : BITFIELD_RANGE(8, 11);
        uint32_t Scoreboard1DeltaY : BITFIELD_RANGE(12, 15);
        uint32_t Scoreboard2DeltaX : BITFIELD_RANGE(16, 19);
        uint32_t Scoreboard2DeltaY : BITFIELD_RANGE(20, 23);
        uint32_t Scoreboard3DeltaX : BITFIELD_RANGE(24, 27);
        uint32_t Scoreboard3DeltaY : BITFIELD_RANGE(28, 31);
        //DW8
        uint32_t Scoreboard4DeltaX : BITFIELD_RANGE(0, 3);
        uint32_t Scoreboard4DeltaY : BITFIELD_RANGE(4, 7);
        uint32_t Scoreboard5DeltaX : BITFIELD_RANGE(8, 11);
        uint32_t Scoreboard5DeltaY : BITFIELD_RANGE(12, 15);
        uint32_t Scoreboard6DeltaX : BITFIELD_RANGE(16, 19);
        uint32_t Scoreboard6DeltaY : BITFIELD_RANGE(20, 23);
        uint32_t Scoreboard7DeltaX : BITFIELD_RANGE(24, 27);
        uint32_t Scoreboard7DeltaY : BITFIELD_RANGE(28, 31);
    } Bitfield;
    enum {
        DWORD_LENGTH_DWORD_COUNT_N = 0x7,
        SUBOPCODE_MEDIA_VFE_STATE_SUBOP = 0x0,
        MEDIA_COMMAND_OPCODE_MEDIA_VFE_STATE = 0x0,
        PIPELINE_MEDIA = 0x2,
        COMMAND_TYPE_GFXPIPE = 0x3,
        RESET_GATEWAY_TIMER_MAINTAINING_THE_EXISTING_TIMESTAMP_STATE = 0x0,
        SLICE_DISABLE_ALL_SUBSLICES_ENABLED = 0x0,
        SCOREBOARD_TYPE_STALLING_SCOREBOARD = 0x0
    };
    static MEDIA_VFE_STATE init() {
        MEDIA_VFE_STATE state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield.Subopcode = SUBOPCODE_MEDIA_VFE_STATE_SUBOP;
        state.Bitfield.MediaCommandOpcode = MEDIA_COMMAND_OPCODE_MEDIA_VFE_STATE;
        state.Bitfield.Pipeline = PIPELINE_MEDIA;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        state.Bitfield.ResetGatewayTimer = RESET_GATEWAY_TIMER_MAINTAINING_THE_EXISTING_TIMESTAMP_STATE;
        state.Bitfield.SliceDisable = SLICE_DISABLE_ALL_SUBSLICES_ENABLED;
        state.Bitfield.ScoreboardType = SCOREBOARD_TYPE_STALLING_SCOREBOARD;
        return state;
    }
};

// Sampler State was added in GEN11
struct STATE_BASE_ADDRESS {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1+DW2
        uint64_t GeneralStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_33 : BITFIELD_RANGE(1, 3);
        uint64_t GeneralStateMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t GeneralStateMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_43 : BITFIELD_RANGE(11, 11);
        uint64_t GeneralStateBaseAddress : BITFIELD_RANGE(12, 63);
        //DW3
        uint32_t Reserved_96 : BITFIELD_RANGE(0, 15);
        uint32_t StatelessDataPortAccessMemoryObjectControlState_Reserved : BITFIELD_RANGE(16, 16);
        uint32_t StatelessDataPortAccessMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(17, 22);
        uint32_t Reserved_119 : BITFIELD_RANGE(23, 31);
        //DW4+DW5
        uint64_t SurfaceStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_129 : BITFIELD_RANGE(1, 3);
        uint64_t SurfaceStateMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t SurfaceStateMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_139 : BITFIELD_RANGE(11, 11);
        uint64_t SurfaceStateBaseAddress : BITFIELD_RANGE(12, 63);
        //DW6+DW7
        uint64_t DynamicStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_193 : BITFIELD_RANGE(1, 3);
        uint64_t DynamicStateMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t DynamicStateMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_203 : BITFIELD_RANGE(11, 11);
        uint64_t DynamicStateBaseAddress : BITFIELD_RANGE(12, 63);
        //DW8+DW9
        uint64_t IndirectObjectBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_257 : BITFIELD_RANGE(1, 3);
        uint64_t IndirectObjectMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t IndirectObjectMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_267 : BITFIELD_RANGE(11, 11);
        uint64_t IndirectObjectBaseAddress : BITFIELD_RANGE(12, 63);
        //DW10+DW11
        uint64_t InstructionBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_321 : BITFIELD_RANGE(1, 3);
        uint64_t InstructionMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t InstructionMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_331 : BITFIELD_RANGE(11, 11);
        uint64_t InstructionBaseAddress : BITFIELD_RANGE(12, 63);
        //DW12
        uint32_t GeneralStateBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_385 : BITFIELD_RANGE(1, 11);
        uint32_t GeneralStateBufferSize : BITFIELD_RANGE(12, 31);
        //DW13
        uint32_t DynamicStateBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_417 : BITFIELD_RANGE(1, 11);
        uint32_t DynamicStateBufferSize : BITFIELD_RANGE(12, 31);
        //DW14
        uint32_t IndirectObjectBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_449 : BITFIELD_RANGE(1, 11);
        uint32_t IndirectObjectBufferSize : BITFIELD_RANGE(12, 31);
        //DW15
        uint32_t InstructionBufferSizeModifyEnable : BITFIELD_RANGE(0, 0);
        uint32_t Reserved_481 : BITFIELD_RANGE(1, 11);
        uint32_t InstructionBufferSize : BITFIELD_RANGE(12, 31);
        //DW16+DW17
        uint64_t BindlessSurfaceStateBaseAddressModifyEnable : BITFIELD_RANGE(0, 0);
        uint64_t Reserved_513 : BITFIELD_RANGE(1, 3);
        uint64_t BindlessSurfaceStateMemoryObjectControlState_Reserved : BITFIELD_RANGE(4, 4);
        uint64_t BindlessSurfaceStateMemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(5, 10);
        uint64_t Reserved_523 : BITFIELD_RANGE(11, 11);
        uint64_t BindlessSurfaceStateBaseAddress : BITFIELD_RANGE(12, 63);
        //DW18
        uint32_t Reserved_576 : BITFIELD_RANGE(0, 11);
        uint32_t BindlessSurfaceStateSize : BITFIELD_RANGE(12, 31);
    } Bitfield;
    enum {
        DWORD_LENGTH_DWORD_COUNT_N = 0x11,
        _3D_COMMAND_SUB_OPCODE_STATE_BASE_ADDRESS = 0x1,
        _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED = 0x1,
        COMMAND_SUBTYPE_GFXPIPE_COMMON = 0x0,
        COMMAND_TYPE_GFXPIPE = 0x3
    };
    static STATE_BASE_ADDRESS init() {
        STATE_BASE_ADDRESS state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_DWORD_COUNT_N;
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_STATE_BASE_ADDRESS;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_COMMON;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};

// GEN11 too
struct GPGPU_CSR_BASE_ADDRESS {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1+DW2
        uint64_t Reserved_32 : BITFIELD_RANGE(0, 11);
        uint64_t GpgpuCsrBaseAddress : BITFIELD_RANGE(12, 63);
    } Bitfield;
    enum {
        DWORD_LENGTH_UNNAMED_1 = 0x1,
        _3D_COMMAND_SUB_OPCODE_GPGPU_CSR_BASE_ADDRESS = 0x4,
        _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED = 0x1,
        COMMAND_SUBTYPE_GFXPIPE_COMMON = 0x0,
        COMMAND_TYPE_GFXPIPE = 0x3
    };
    static GPGPU_CSR_BASE_ADDRESS init() {
        GPGPU_CSR_BASE_ADDRESS state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_UNNAMED_1;
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_GPGPU_CSR_BASE_ADDRESS;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_COMMON;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};

// GEN11 too
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

// GEN11 too
struct GPGPU_WALKER {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t PredicateEnable : BITFIELD_RANGE(8, 8);
        uint32_t Reserved_9 : BITFIELD_RANGE(9, 9);
        uint32_t IndirectParameterEnable : BITFIELD_RANGE(10, 10);
        uint32_t Reserved_11 : BITFIELD_RANGE(11, 15);
        uint32_t Subopcode : BITFIELD_RANGE(16, 23);
        uint32_t MediaCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t Pipeline : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
        uint32_t InterfaceDescriptorOffset : BITFIELD_RANGE(0, 5);
        uint32_t Reserved_38 : BITFIELD_RANGE(6, 31);
        //DW2
        uint32_t IndirectDataLength : BITFIELD_RANGE(0, 16);
        uint32_t Reserved_81 : BITFIELD_RANGE(17, 31);
        //DW3
        uint32_t Reserved_96 : BITFIELD_RANGE(0, 5);
        uint32_t IndirectDataStartAddress : BITFIELD_RANGE(6, 31);
        //DW4
        uint32_t ThreadWidthCounterMaximum : BITFIELD_RANGE(0, 5);
        uint32_t Reserved_134 : BITFIELD_RANGE(6, 7);
        uint32_t ThreadHeightCounterMaximum : BITFIELD_RANGE(8, 13);
        uint32_t Reserved_142 : BITFIELD_RANGE(14, 15);
        uint32_t ThreadDepthCounterMaximum : BITFIELD_RANGE(16, 21);
        uint32_t Reserved_150 : BITFIELD_RANGE(22, 29);
        uint32_t SimdSize : BITFIELD_RANGE(30, 31);
        //DW5 - DW14
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
        INDIRECTDATASTARTADDRESS_INDEX = 0x3,
        INDIRECTDATASTARTADDRESS_ALIGN_SIZE = 0x40
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

// TileCacheFlushEnable was added in GEN11
struct PIPE_CONTROL {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_8 : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
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
        //DW2
        uint32_t Reserved_64 : BITFIELD_RANGE(0, 1);
        uint32_t Address : BITFIELD_RANGE(2, 31);
        //DW3
        uint32_t AddressHigh;
        //DW4+DW5
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
        state.Bitfield.ConstantCacheInvalidationEnable = false;
        state.Bitfield.InstructionCacheInvalidateEnable = false;
        state.Bitfield.PipeControlFlushEnable = false;
        state.Bitfield.RenderTargetCacheFlushEnable = false;
        state.Bitfield.StateCacheInvalidationEnable = false;
        state.Bitfield.TextureCacheInvalidationEnable = false;
        state.Bitfield.VfCacheInvalidationEnable = false;
        state.Bitfield.GenericMediaStateClear = false;
        state.Bitfield.TlbInvalidate = false;
        state.Bitfield.NotifyEnable = false;
        return state;
    }
};

// MediaSamplerPowerClockGateDisable was added in GEN11
struct PIPELINE_SELECT {
    struct TheStructure {
        //DW0
        uint32_t PipelineSelection : BITFIELD_RANGE(0, 1);
        uint32_t RenderSliceCommonPowerGateEnable : BITFIELD_RANGE(2, 2);
        uint32_t RenderSamplerPowerGateEnable : BITFIELD_RANGE(3, 3);
        uint32_t MediaSamplerDopClockGateEnable : BITFIELD_RANGE(4, 4);
        uint32_t ForceMediaAwake : BITFIELD_RANGE(5, 5);
        uint32_t Reserved_6 : BITFIELD_RANGE(6, 7);
        uint32_t MaskBits : BITFIELD_RANGE(8, 15);
        uint32_t _3DCommandSubOpcode : BITFIELD_RANGE(16, 23);
        uint32_t _3DCommandOpcode : BITFIELD_RANGE(24, 26);
        uint32_t CommandSubtype : BITFIELD_RANGE(27, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
    } Bitfield;
    enum {
        PIPELINE_SELECTION_3D = 0x0,
        PIPELINE_SELECTION_MEDIA = 0x1,
        PIPELINE_SELECTION_GPGPU = 0x2,
        _3D_COMMAND_SUB_OPCODE_PIPELINE_SELECT = 0x4,
        _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED = 0x1,
        COMMAND_SUBTYPE_GFXPIPE_SINGLE_DW = 0x1,
        COMMAND_TYPE_GFXPIPE = 0x3
    };
    static PIPELINE_SELECT init() {
        PIPELINE_SELECT state;
        memset(&state, 0, sizeof(state));
        state.Bitfield._3DCommandSubOpcode = _3D_COMMAND_SUB_OPCODE_PIPELINE_SELECT;
        state.Bitfield._3DCommandOpcode = _3D_COMMAND_OPCODE_GFXPIPE_NONPIPELINED;
        state.Bitfield.CommandSubtype = COMMAND_SUBTYPE_GFXPIPE_SINGLE_DW;
        state.Bitfield.CommandType = COMMAND_TYPE_GFXPIPE;
        return state;
    }
};

// AddCsMmioStartOffset was added in GEN11
struct MI_LOAD_REGISTER_IMM {
    struct TheStructure {
        //Dw0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t ByteWriteDisables : BITFIELD_RANGE(8, 11);
        uint32_t Reserved_12 : BITFIELD_RANGE(12, 22);
        uint32_t MiCommandOpcode : BITFIELD_RANGE(23, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //DW1
        uint32_t Reserved_32 : BITFIELD_RANGE(0, 1);
        uint32_t RegisterOffset : BITFIELD_RANGE(2, 22);
        uint32_t Reserved_55 : BITFIELD_RANGE(23, 31);
        //DW2
        uint32_t DataDword;
    } Bitfield;
    enum {
        DWORD_LENGTH_EXCLUDES_DWORD_0_1 = 0x1,
        MI_COMMAND_OPCODE_MI_LOAD_REGISTER_IMM = 0x22,
        COMMAND_TYPE_MI_COMMAND = 0x0
    };
    static MI_LOAD_REGISTER_IMM init() {
        MI_LOAD_REGISTER_IMM state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_EXCLUDES_DWORD_0_1;
        state.Bitfield.MiCommandOpcode = MI_COMMAND_OPCODE_MI_LOAD_REGISTER_IMM;
        state.Bitfield.CommandType = COMMAND_TYPE_MI_COMMAND;
        return state;
    }
};

struct INTERFACE_DESCRIPTOR_DATA {
    struct TheStructure {
        //DW0
        uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
        uint32_t KernelStartPointer : BITFIELD_RANGE(6, 31);
        //DW1
        uint32_t KernelStartPointerHigh : BITFIELD_RANGE(0, 15);
        uint32_t Reserved_48 : BITFIELD_RANGE(16, 31);
        //DW2
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
        //DW3
        uint32_t Reserved_96 : BITFIELD_RANGE(0, 1);
        uint32_t SamplerCount : BITFIELD_RANGE(2, 4);
        uint32_t SamplerStatePointer : BITFIELD_RANGE(5, 31);
        //DW4
        uint32_t BindingTableEntryCount : BITFIELD_RANGE(0, 4);
        uint32_t BindingTablePointer : BITFIELD_RANGE(5, 15);
        uint32_t Reserved_144 : BITFIELD_RANGE(16, 31);
        //DW5
        uint32_t ConstantUrbEntryReadOffset : BITFIELD_RANGE(0, 15);
        uint32_t ConstantIndirectUrbEntryReadLength : BITFIELD_RANGE(16, 31);
        //DW6
        uint32_t NumberOfThreadsInGpgpuThreadGroup : BITFIELD_RANGE(0, 9);
        uint32_t Reserved_202 : BITFIELD_RANGE(10, 14);
        uint32_t GlobalBarrierEnable : BITFIELD_RANGE(15, 15);
        uint32_t SharedLocalMemorySize : BITFIELD_RANGE(16, 20);
        uint32_t BarrierEnable : BITFIELD_RANGE(21, 21);
        uint32_t RoundingMode : BITFIELD_RANGE(22, 23);
        uint32_t Reserved_216 : BITFIELD_RANGE(24, 31);
        //DW7
        uint32_t CrossThreadConstantDataReadLength : BITFIELD_RANGE(0, 7);
        uint32_t Reserved_232 : BITFIELD_RANGE(8, 31); 
    } Bitfield;
    enum {
        FLOATING_POINT_MODE_IEEE_754 = 0x0,
        THREAD_PRIORITY_NORMAL_PRIORITY = 0x0,
        SINGLE_PROGRAM_FLOW_MULTIPLE = 0x0,
        DENORM_MODE_FTZ = 0x0,
        DENORM_MODE_SETBYKERNEL = 0x1,
        SAMPLER_COUNT_NO_SAMPLERS_USED = 0x0,
        SHARED_LOCAL_MEMORY_SIZE_ENCODES_0K = 0x0,
        ROUNDING_MODE_RTNE = 0x0
    };
    static INTERFACE_DESCRIPTOR_DATA init() {
        INTERFACE_DESCRIPTOR_DATA state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.FloatingPointMode = FLOATING_POINT_MODE_IEEE_754;
        state.Bitfield.ThreadPriority = THREAD_PRIORITY_NORMAL_PRIORITY;
        state.Bitfield.SingleProgramFlow = SINGLE_PROGRAM_FLOW_MULTIPLE;
        state.Bitfield.DenormMode = DENORM_MODE_FTZ;
        state.Bitfield.SamplerCount = SAMPLER_COUNT_NO_SAMPLERS_USED;
        state.Bitfield.SharedLocalMemorySize = SHARED_LOCAL_MEMORY_SIZE_ENCODES_0K;
        state.Bitfield.RoundingMode = ROUNDING_MODE_RTNE;
        return state;
    }
};

struct BINDING_TABLE_STATE {
    struct TheStructure {
        uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
        uint32_t SurfaceStatePointer : BITFIELD_RANGE(6, 31);
    } Bitfield;
    enum {
        SURFACESTATEPOINTER_BYTEOFFSET = 0x0,
        SURFACESTATEPOINTER_INDEX = 0x0,
        SURFACESTATEPOINTER_BIT_SHIFT = 0x6,
        SURFACESTATEPOINTER_ALIGN_SIZE = 0x40
    };
    static BINDING_TABLE_STATE init() {
        BINDING_TABLE_STATE state;
        memset(&state, 0, sizeof(state));
        return state;
    }
};

struct RENDER_SURFACE_STATE {
    struct TheStructure {
        //DW0
        uint32_t Reserved_0 : BITFIELD_RANGE(0, 5);
        uint32_t MediaBoundaryPixelMode : BITFIELD_RANGE(6, 7);
        uint32_t RenderCacheReadWriteMode : BITFIELD_RANGE(8, 8);
        uint32_t SamplerL2OutOfOrderModeDisable : BITFIELD_RANGE(9, 9);
        uint32_t VerticalLineStrideOffset : BITFIELD_RANGE(10, 10);
        uint32_t VerticalLineStride : BITFIELD_RANGE(11, 11);
        uint32_t TileMode : BITFIELD_RANGE(12, 13);
        uint32_t SurfaceHorizontalAlignment : BITFIELD_RANGE(14, 15);
        uint32_t SurfaceVerticalAlignment : BITFIELD_RANGE(16, 17);
        uint32_t SurfaceFormat : BITFIELD_RANGE(18, 26);
        uint32_t Astc_Enable : BITFIELD_RANGE(27, 27);
        uint32_t SurfaceArray : BITFIELD_RANGE(28, 28);
        uint32_t SurfaceType : BITFIELD_RANGE(29, 31);
        //DW1
        uint32_t SurfaceQpitch : BITFIELD_RANGE(0, 14);
        uint32_t Reserved_47 : BITFIELD_RANGE(15, 18);
        uint32_t BaseMipLevel : BITFIELD_RANGE(19, 23);
        uint32_t MemoryObjectControlState_Reserved : BITFIELD_RANGE(24, 24);
        uint32_t MemoryObjectControlState_IndexToMocsTables : BITFIELD_RANGE(25, 30);
        uint32_t Reserved_63 : BITFIELD_RANGE(31, 31);
        //DW2
        uint32_t Width : BITFIELD_RANGE(0, 13);
        uint32_t Reserved_78 : BITFIELD_RANGE(14, 15);
        uint32_t Height : BITFIELD_RANGE(16, 29);
        uint32_t Reserved_94 : BITFIELD_RANGE(30, 31);
        //DW3
        uint32_t SurfacePitch : BITFIELD_RANGE(0, 17);
        uint32_t Reserved_114 : BITFIELD_RANGE(18, 20);
        uint32_t Depth : BITFIELD_RANGE(21, 31);
        //DW4
        uint32_t MultisamplePositionPaletteIndex : BITFIELD_RANGE(0, 2);
        uint32_t NumberOfMultisamples : BITFIELD_RANGE(3, 5);
        uint32_t MultisampledSurfaceStorageFormat : BITFIELD_RANGE(6, 6);
        uint32_t RenderTargetViewExtent : BITFIELD_RANGE(7, 17);
        uint32_t MinimumArrayElement : BITFIELD_RANGE(18, 28);
        uint32_t RenderTargetAndSampleUnormRotation : BITFIELD_RANGE(29, 30);
        uint32_t Reserved_159 : BITFIELD_RANGE(31, 31);
        //DW5
        uint32_t MipCountLod : BITFIELD_RANGE(0, 3);
        uint32_t SurfaceMinLod : BITFIELD_RANGE(4, 7);
        uint32_t MipTailStartLod : BITFIELD_RANGE(8, 11);
        uint32_t Reserved_172 : BITFIELD_RANGE(12, 13);
        uint32_t CoherencyType : BITFIELD_RANGE(14, 14);
        uint32_t Reserved_175 : BITFIELD_RANGE(15, 17);
        uint32_t TiledResourceMode : BITFIELD_RANGE(18, 19);
        uint32_t EwaDisableForCube : BITFIELD_RANGE(20, 20);
        uint32_t YOffset : BITFIELD_RANGE(21, 23);
        uint32_t Reserved_184 : BITFIELD_RANGE(24, 24);
        uint32_t XOffset : BITFIELD_RANGE(25, 31);
        //DW6 (if _SurfaceFormatIsnotPlanar)
        uint32_t AuxiliarySurfaceMode : BITFIELD_RANGE(0, 2);
        uint32_t AuxiliarySurfacePitch : BITFIELD_RANGE(3, 11);
        uint32_t Reserved_204 : BITFIELD_RANGE(12, 15);
        uint32_t AuxiliarySurfaceQpitch : BITFIELD_RANGE(16, 30);
        uint32_t Reserved_223 : BITFIELD_RANGE(31, 31);
        //DW7
        uint32_t ResourceMinLod : BITFIELD_RANGE(0, 11);
        uint32_t Reserved_236 : BITFIELD_RANGE(12, 15);
        uint32_t ShaderChannelSelectAlpha : BITFIELD_RANGE(16, 18);
        uint32_t ShaderChannelSelectBlue : BITFIELD_RANGE(19, 21);
        uint32_t ShaderChannelSelectGreen : BITFIELD_RANGE(22, 24);
        uint32_t ShaderChannelSelectRed : BITFIELD_RANGE(25, 27);
        uint32_t Reserved_252 : BITFIELD_RANGE(28, 29);
        uint32_t MemoryCompressionEnable : BITFIELD_RANGE(30, 30);
        uint32_t MemoryCompressionMode : BITFIELD_RANGE(31, 31);
        //DW8
        uint64_t SurfaceBaseAddress;
        uint64_t QuiltWidth : BITFIELD_RANGE(0, 4);
        uint64_t QuiltHeight : BITFIELD_RANGE(5, 9);
        uint64_t Reserved_330 : BITFIELD_RANGE(10, 63);
        uint32_t Reserved_384;
        uint32_t Reserved_416;
        uint32_t Reserved_448;
        uint32_t Reserved_480;
    } Bitfield;
    enum {
        MEDIA_BOUNDARY_PIXEL_MODE_NORMAL_MODE = 0x0,
        RENDER_CACHE_READ_WRITE_MODE_WRITE_ONLY_CACHE = 0x0,
        TILE_MODE_LINEAR = 0x0,
        SURFACE_HORIZONTAL_ALIGNMENT_HALIGN_4 = 0x1,
        SURFACE_VERTICAL_ALIGNMENT_VALIGN_4 = 0x1,
        SURFACE_TYPE_SURFTYPE_1D = 0x0,
        SURFACE_TYPE_SURFTYPE_BUFFER = 0x4,
        SURFACE_FORMAT_RAW = 0x1ff,
        COHERENCY_TYPE_GPU_COHERENT = 0x0,
        COHERENCY_TYPE_IA_COHERENT = 0x1,
        TILED_RESOURCE_MODE_NONE = 0x0,
        SHADER_CHANNEL_SELECT_ZERO = 0x0,
        MEMORY_COMPRESSION_MODE_HORIZONTAL = 0x0,
        NUMBER_OF_MULTISAMPLES_MULTISAMPLECOUNT_1 = 0x0,
        MULTISAMPLED_SURFACE_STORAGE_FORMAT_MSS = 0x0,
        RENDER_TARGET_AND_SAMPLE_UNORM_ROTATION_0DEG = 0x0,
        AUXILIARY_SURFACE_MODE_AUX_NONE = 0x0
    };
    static RENDER_SURFACE_STATE init() {
        RENDER_SURFACE_STATE state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.MediaBoundaryPixelMode = MEDIA_BOUNDARY_PIXEL_MODE_NORMAL_MODE;
        state.Bitfield.RenderCacheReadWriteMode = RENDER_CACHE_READ_WRITE_MODE_WRITE_ONLY_CACHE;
        state.Bitfield.TiledResourceMode = TILED_RESOURCE_MODE_NONE;
        state.Bitfield.ShaderChannelSelectAlpha = SHADER_CHANNEL_SELECT_ZERO;
        state.Bitfield.ShaderChannelSelectBlue = SHADER_CHANNEL_SELECT_ZERO;
        state.Bitfield.ShaderChannelSelectGreen = SHADER_CHANNEL_SELECT_ZERO;
        state.Bitfield.ShaderChannelSelectRed = SHADER_CHANNEL_SELECT_ZERO;
        state.Bitfield.MemoryCompressionMode = MEMORY_COMPRESSION_MODE_HORIZONTAL;
        state.Bitfield.NumberOfMultisamples = NUMBER_OF_MULTISAMPLES_MULTISAMPLECOUNT_1;
        state.Bitfield.MultisampledSurfaceStorageFormat = MULTISAMPLED_SURFACE_STORAGE_FORMAT_MSS;
        state.Bitfield.RenderTargetAndSampleUnormRotation = RENDER_TARGET_AND_SAMPLE_UNORM_ROTATION_0DEG;
        state.Bitfield.AuxiliarySurfaceMode = AUXILIARY_SURFACE_MODE_AUX_NONE;

        state.Bitfield.SurfaceType = SURFACE_TYPE_SURFTYPE_BUFFER;
        state.Bitfield.SurfaceFormat = SURFACE_FORMAT_RAW;
        state.Bitfield.SurfaceVerticalAlignment = SURFACE_VERTICAL_ALIGNMENT_VALIGN_4;
        state.Bitfield.SurfaceHorizontalAlignment = SURFACE_HORIZONTAL_ALIGNMENT_HALIGN_4;
        state.Bitfield.TileMode = TILE_MODE_LINEAR;
        state.Bitfield.CoherencyType = COHERENCY_TYPE_IA_COHERENT;

        return state;
    }
};

union SURFACE_STATE_BUFFER_LENGTH {
    uint32_t Length;
    struct SurfaceState {
        uint32_t Width : BITFIELD_RANGE(0, 6);
        uint32_t Height : BITFIELD_RANGE(7, 20);
        uint32_t Depth : BITFIELD_RANGE(21, 31);
    } SurfaceState;
};

struct MI_BATCH_BUFFER_START {
    struct TheStructure {
        //DW0
        uint32_t DwordLength : BITFIELD_RANGE(0, 7);
        uint32_t AddressSpaceIndicator : BITFIELD_RANGE(8, 8);
        uint32_t Reserved_9 : BITFIELD_RANGE(9, 9);
        uint32_t ResourceStreamerEnable : BITFIELD_RANGE(10, 10);
        uint32_t Reserved_11 : BITFIELD_RANGE(11, 14);
        uint32_t PredicationEnable : BITFIELD_RANGE(15, 15);
        uint32_t AddOffsetEnable : BITFIELD_RANGE(16, 16);
        uint32_t Reserved_17 : BITFIELD_RANGE(17, 21);
        uint32_t SecondLevelBatchBuffer : BITFIELD_RANGE(22, 22);
        uint32_t MiCommandOpcode : BITFIELD_RANGE(23, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
        //Dw1+DW2
        uint64_t Reserved_32 : BITFIELD_RANGE(0, 1);
        uint64_t BatchBufferStartAddress_Graphicsaddress47_2 : BITFIELD_RANGE(2, 47);
        uint64_t BatchBufferStartAddress_Reserved : BITFIELD_RANGE(48, 63);
    } Bitfield;
    enum {
        DWORD_LENGTH_EXCLUDES_DWORD_0_1 = 0x1,
        ADDRESS_SPACE_INDICATOR_GGTT = 0x0,
        ADDRESS_SPACE_INDICATOR_PPGTT = 0x1,
        SECOND_LEVEL_BATCH_BUFFER_FIRST_LEVEL_BATCH = 0x0,
        MI_COMMAND_OPCODE_MI_BATCH_BUFFER_START = 0x31,
        COMMAND_TYPE_MI_COMMAND = 0x0
    };
    static MI_BATCH_BUFFER_START init() {
        MI_BATCH_BUFFER_START state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.DwordLength = DWORD_LENGTH_EXCLUDES_DWORD_0_1;
        state.Bitfield.AddressSpaceIndicator = ADDRESS_SPACE_INDICATOR_GGTT;
        state.Bitfield.SecondLevelBatchBuffer = SECOND_LEVEL_BATCH_BUFFER_FIRST_LEVEL_BATCH;
        state.Bitfield.MiCommandOpcode = MI_COMMAND_OPCODE_MI_BATCH_BUFFER_START;
        state.Bitfield.CommandType = COMMAND_TYPE_MI_COMMAND;
        return state;
    }
};

struct MI_BATCH_BUFFER_END {
    struct TheStructure {
        //DW0
        uint32_t Reserved_0 : BITFIELD_RANGE(0, 22);
        uint32_t MiCommandOpcode : BITFIELD_RANGE(23, 28);
        uint32_t CommandType : BITFIELD_RANGE(29, 31);
    } Bitfield;
    enum {
        MI_COMMAND_OPCODE_MI_BATCH_BUFFER_END = 0xa,
        COMMAND_TYPE_MI_COMMAND = 0x0
    };
    static MI_BATCH_BUFFER_END init() {
        MI_BATCH_BUFFER_END state;
        memset(&state, 0, sizeof(state));
        state.Bitfield.MiCommandOpcode = MI_COMMAND_OPCODE_MI_BATCH_BUFFER_END;
        state.Bitfield.CommandType = COMMAND_TYPE_MI_COMMAND;
        return state;
    }
};

#pragma pack()





