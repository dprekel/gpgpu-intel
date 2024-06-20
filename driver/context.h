#pragma once

#include <stdio.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include "device.h"
#include "gpgpu.h"
#include "kernel.h"

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
    constexpr uint64_t megaByte = 1024 * kiloByte;
    constexpr size_t pageSize = 4 * kiloByte;
    constexpr size_t cacheLineSize = 64; 
};

enum class DebugPauseState : uint32_t {
    disabled,
    waitingForFirstSemaphore
};

enum ThreadArbitrationPolicy {
    AgeBased = 0x0,
    RoundRobin = 0x1,
    RoundRobinAfterDependency = 0x2,
    NotPresent = 0xffff
};

struct BufferObject {
    BufferObject() {}
    ~BufferObject() {
        printf("BufferObject destructor called!\n");
    }
    template <typename T>
    T ptrOffset(size_t ptr_offset) {
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(cpuAddress);
        offset += sizeof(ptr_offset);
        return reinterpret_cast<T>(baseAddr + offset);
    }
    int bufferType      = 0;
    void* cpuAddress    = nullptr;
    uint64_t gpuAddress = 0;
    size_t offset       = 0;
    size_t size         = 0;
    uint32_t handle     = 0;
};

struct AllocationData {
    uint64_t kernelAddress;
    uint64_t scratchAddress;
    uint64_t tagAddress;
    uint64_t preemptionAddress;

    size_t hwThreadsPerWorkGroup;
    uint32_t maxVfeThreads;
    uint32_t perThreadScratchSpace;
};

class Buffer : public pBuffer {
  public:
    Buffer(BufferObject* dataBuffer);
    ~Buffer();
    uint64_t gpuAddress = 0;
    size_t size = 0;
};


class Context : public pContext {
  public:
    Context(Device* device);
    ~Context();
    void setKernelData(KernelFromPatchtokens* kernelData);
    KernelFromPatchtokens* getKernelData();
    void setMaxWorkGroupSize();
    int createDrmContext();
    void setNonPersistentContext();
    BufferObject* allocateBufferObject(size_t size);
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size);
    int allocateISAMemory();
    int createScratchAllocation();
    int createSurfaceStateHeap();
    int createIndirectObjectHeap();
    int createDynamicStateHeap();
    int createPreemptionAllocation();
    int createTagAllocation();
    int createCommandBuffer();

    Device* device;
    Kernel* kernel = nullptr;
  private:
    void generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize);

    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;
    std::vector<std::unique_ptr<BufferObject>> execBuffer;
    AllocationData allocData;
    uint32_t vmId;
    uint32_t ctxId;

    uint32_t workDim;
    uint32_t maxWorkItemsPerWorkGroup;
    size_t workItemsPerWorkGroup[3];
    size_t globalWorkItems[3];
    size_t numWorkGroups[3];
};



