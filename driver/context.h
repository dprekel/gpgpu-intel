#pragma once

#include <stdio.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include "device.h"
#include "gpgpu.h"
#include "ioctl.h"
#include "kernel.h"
#include "utils.h"

#define BITFIELD_RANGE(startbit, endbit) ((endbit) - (startbit) + 1)

class Device;
class Kernel;

enum BufferType {
    BUFFER_HOST_MEMORY,
    KERNEL_ISA,
    KERNEL_ISA_INTERNAL,
    SCRATCH_SURFACE,
    PREEMPTION,
    TAG_BUFFER,
    INTERNAL_HEAP,
    LINEAR_STREAM,
    COMMAND_BUFFER
};

namespace MemoryConstants {
    constexpr uint64_t kiloByte = 1024;
    constexpr uint64_t kiloByteShiftSize = 10;
    constexpr uint64_t megaByte = 1024 * kiloByte;
    constexpr uint64_t gigaByte = 1024 * megaByte;
    constexpr uint64_t cpuVirtualAddressSize = 48;
    constexpr size_t pageSize = 4 * kiloByte;
    constexpr size_t pageSize64k = 64 * kiloByte;
    constexpr size_t cacheLineSize = 64;
    constexpr uint32_t sizeOf4GBinPageEntities = (gigaByte * 4 - pageSize) / pageSize;
};

namespace MMIOAddresses {
    constexpr uint32_t L3Register = 0x7034;
    constexpr uint32_t debugControlReg2 = 0xe404;
    constexpr uint32_t preemptConfigReg = 0x2580;
};

enum class DebugPauseState : uint32_t {
    disabled,
    waitingForFirstSemaphore
};

enum ThreadArbitrationPolicy {
    AgeBased = 0x0,
    RoundRobin = 0x100,
    RoundRobinAfterDependency = 0x0,
    NotPresent = 0x0
};

struct BufferObject {
    BufferObject(int fd, int bufferType, void* cpuAddress, uint32_t handle, size_t size);
    ~BufferObject();
    template <typename T>
    T ptrOffset(size_t ptr_offset) {
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(cpuAddress);
        uintptr_t newAddr = baseAddr + offset;
        offset += ptr_offset;
        return reinterpret_cast<T>(newAddr);
    }
    void deleteHandle();

    int fd = 0;
    int bufferType = 0;
    void* cpuAddress = nullptr;
    uint64_t gpuBaseAddress = 0u;
    uint64_t gpuAddress = 0u;
    uint32_t handle = 0u;
    size_t offset = 0u;
    size_t size = 0u;
};


class Context : public pContext {
  public:
    Context(Device* device);
    ~Context();
    void setKernelData(KernelFromPatchtokens* kernelData);
    KernelFromPatchtokens* getKernelData();
    BufferObject* getBatchBuffer();
    bool isSIPKernelAllocated();
    void setMaxWorkGroupSize();
    void setMaxThreadsForVfe();
    std::unique_ptr<BufferObject> allocateBufferObject(size_t size, int bufferType);
    int createDRMContext();
    int allocateReusableBufferObjects();
    int createSipAllocation(size_t sipSize, const char* sipBinaryRaw);
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size);
    int constructBufferObjects();
    int populateAndSubmitExecBuffer();
    int exec(drm_i915_gem_exec_object2* execObjects, BufferObject** execBufferPtrs, size_t residencyCount, size_t batchSize);
    int finishExecution();

    Device* device;
    Kernel* kernel = nullptr;

  private:
    bool isGraphicsBaseAddressRequired(int bufferType);
    int allocateISAMemory();
    int createScratchAllocation();
    int createSurfaceStateHeap();
    int createIndirectObjectHeap();
    int createDynamicStateHeap();
    //int createCommandStreamTask();
    int createCommandStreamReceiver();
    void patchKernelConstant(const PatchDataParameterBuffer* info, char* crossThreadData, size_t kernelConstant);
    void generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize);
    void generateLocalIDs(BufferObject* ioh);
    void alignToCacheLine(BufferObject* commandBuffer);
    void fillExecObject(drm_i915_gem_exec_object2& execObject, BufferObject* bo);

    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;

    //TODO: Check everything in scratchAllocation
    std::unique_ptr<BufferObject> preemptionAllocation;
    std::unique_ptr<BufferObject> tagAllocation;
    std::unique_ptr<BufferObject> dataBatchBuffer;
    std::unique_ptr<BufferObject> kernelAllocation;
    std::unique_ptr<BufferObject> scratchAllocation;    //TODO: Memory leak here
    std::unique_ptr<BufferObject> sshAllocation;
    std::unique_ptr<BufferObject> iohAllocation;
    std::unique_ptr<BufferObject> dshAllocation;
    std::unique_ptr<BufferObject> sipAllocation;
    //std::unique_ptr<BufferObject> commandStreamTask;
    std::unique_ptr<BufferObject> commandStreamCSR;

    std::vector<BufferObject*> execBuffer;
    std::vector<drm_i915_gem_exec_object2> execObjects;

    uint32_t ctxId = 0u;
    uint32_t vmId = 0u;
    uint32_t workDim = 0u;
    size_t workItemsPerWorkGroup[3];
    size_t globalWorkItems[3];
    size_t numWorkGroups[3];

    size_t hwThreadsPerWorkGroup = 0u;
    uint32_t maxWorkItemsPerWorkGroup = 0u;
    uint32_t maxVfeThreads = 0u;
    uint32_t perThreadScratchSpace = 0u;
    uint32_t GRFSize = 0u;
    uint32_t crossThreadDataSize = 0u;
    uint32_t perThreadDataSize = 0u;
    uint32_t completionTag = 0u;
    bool isMidThreadLevelPreemptionSupported = false;
    bool isSipKernelAllocated = false;
};



// GEN9 hardware has 64 16bit control registers for L3 caching policy and 64 32bit control
// registers for LLC/eDRAM caching policy. Each index into the two register files
// represents a unique Memory Object Control State (MOCS). In STATE_BASE_ADDRESS and
// RENDER_SURFACE_STATE you can specify the caching policy of a buffer object by writing a
// MOCS index into the corresponding field. More information about the registers and
// the 64 MOCS indices can be found in the hardware documentation.
enum MOCS {
    PageTableControlledCaching = 0x0,
    AggressiveCaching = 0x2
};

// These registers are not used directly in this code, but indirectly through MOCS index:
struct L3_CONTROL_REG {
    uint16_t EnableSkipCaching : BITFIELD_RANGE(0, 0);
    uint16_t SkipCacheabilityControl : BITFIELD_RANGE(1, 3);
    uint16_t L3CacheabilityControl : BITFIELD_RANGE(4, 5);
    uint16_t Reserved : BITFIELD_RANGE(6, 16);
};

struct LLC_EDRAM_CONTROL_REG {
    uint32_t LLCeDRAMCacheabilityControl : BITFIELD_RANGE(0, 1);
    uint32_t TargetCache : BITFIELD_RANGE(2, 3);
    uint32_t CacheReplacementManagement : BITFIELD_RANGE(4, 5);
    uint32_t DontAllocateOnMiss : BITFIELD_RANGE(6, 6);
    uint32_t EnableReverseSkipCaching : BITFIELD_RANGE(7, 7);
    uint32_t SkipCacheabilityControl : BITFIELD_RANGE(8, 10);
    uint32_t PageFaultingMode : BITFIELD_RANGE(11, 13);
    uint32_t SnoopControlField : BITFIELD_RANGE(14, 14);
    uint32_t ClassOfService : BITFIELD_RANGE(15, 16);
    uint32_t Reserved : BITFIELD_RANGE(17, 31);
};

