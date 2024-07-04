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
    constexpr size_t pageSize = 4 * kiloByte;
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
    BufferObject();
    ~BufferObject();
    template <typename T>
    T ptrOffset(size_t ptr_offset) {
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(cpuAddress);
        uintptr_t newAddr = baseAddr + offset;
        offset += ptr_offset;
        return reinterpret_cast<T>(newAddr);
    }
    int bufferType = 0;
    void* cpuAddress = nullptr;
    uint64_t gpuBaseAddress = 0u;
    uint64_t gpuAddress = 0u;
    size_t offset = 0u;
    size_t size = 0u;
    uint32_t handle = 0u;
};


class Context : public pContext {
  public:
    Context(Device* device);
    ~Context();
    void setKernelData(KernelFromPatchtokens* kernelData);
    KernelFromPatchtokens* getKernelData();
    bool getIsSipKernelAllocated();
    void setMaxWorkGroupSize();
    void setMaxThreadsForVfe();
    int createDRMContext();
    std::unique_ptr<BufferObject> allocateBufferObject(size_t size, int bufferType);
    int createTagAllocation();
    int createPreemptionAllocation();
    int createSipAllocation(size_t sipSize, const char* sipBinaryRaw);
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size);
    int createGPUAllocations();
    uint32_t getMocsIndex();
    int populateAndSubmitExecBuffer();
    int exec(drm_i915_gem_exec_object2* execObjects, BufferObject** execBufferPtrs, size_t residencyCount, size_t used);
    int finishExecution();

    Device* device;
    Kernel* kernel = nullptr;
    CIFMain* igcMain = nullptr;
    CIFMain* fclMain = nullptr;

  private:
    int allocateISAMemory();
    int createScratchAllocation();
    int createSurfaceStateHeap();
    int createIndirectObjectHeap();
    int createDynamicStateHeap();
    int createCommandStreamTask();
    int createCommandStreamReceiver();
    void patchKernelConstant(const PatchDataParameterBuffer* info, char* crossThreadData, size_t kernelConstant);
    void generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize);
    void generateLocalIDs(BufferObject* ioh);
    void alignToCacheLine(BufferObject* commandBuffer);
    void fillExecObject(drm_i915_gem_exec_object2& execObject, BufferObject* bo);

    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;

    //TODO: How to set and save data buffers?
    //TODO: Check allocation sizes for ssh, ioh, dsh and command buffer
    //TODO: Check everything in scratchAllocation
    std::unique_ptr<BufferObject> kernelAllocation;
    std::unique_ptr<BufferObject> scratchAllocation;    //TODO: Memory leak here
    std::unique_ptr<BufferObject> sshAllocation;
    std::unique_ptr<BufferObject> iohAllocation;
    std::unique_ptr<BufferObject> dshAllocation;
    std::unique_ptr<BufferObject> tagAllocation;
    std::unique_ptr<BufferObject> preemptionAllocation;
    std::unique_ptr<BufferObject> sipAllocation;
    std::unique_ptr<BufferObject> commandStreamTask;
    std::unique_ptr<BufferObject> commandStreamCSR;

    std::vector<BufferObject*> execBuffer;
    std::vector<drm_i915_gem_exec_object2> execObjects;

    uint32_t ctxId = 0u;
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
    bool isSipKernelAllocated = false;
};



