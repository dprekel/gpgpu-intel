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
    constexpr uint64_t kiloByteShiftSize = 10;
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
    uint64_t scratchAddress;
    uint64_t commandStreamTaskAddress;

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
    bool getIsSipKernelAllocated();
    void setMaxWorkGroupSize();
    void setMaxThreadsForVfe();
    int createDrmContext();
    void setNonPersistentContext();
    std::unique_ptr<BufferObject> allocateBufferObject(size_t size);
    int createTagAllocation();
    int createPreemptionAllocation();
    int createSipAllocation(size_t sipSize, const char* sipBinaryRaw);
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size);
    int createGPUAllocations();

    Device* device;
    Kernel* kernel = nullptr;
  private:
    int allocateISAMemory();
    int createScratchAllocation();
    int createSurfaceStateHeap();
    int createIndirectObjectHeap();
    int createDynamicStateHeap();
    int createCommandStreamTask();
    int createCommandStreamReceiver();
    void generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize);

    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;

    //TODO: Load compilers in CreateDevices
    //TODO: Add virtual memory creation to CreateContext
    //TODO: How to set and save data buffers?
    std::unique_ptr<BufferObject> kernelAllocation;
    std::unique_ptr<BufferObject> scratchAllocation;
    std::unique_ptr<BufferObject> sshAllocation;
    std::unique_ptr<BufferObject> iohAllocation;
    std::unique_ptr<BufferObject> dshAllocation;
    std::unique_ptr<BufferObject> tagAllocation;
    std::unique_ptr<BufferObject> preemptionAllocation;
    std::unique_ptr<BufferObject> sipAllocation;
    std::unique_ptr<BufferObject> commandStreamTask;
    std::unique_ptr<BufferObject> commandStreamCSR;
    std::vector<std::unique_ptr<BufferObject>> execBuffer;
    AllocationData allocData;
    uint32_t vmId;
    uint32_t ctxId;

    uint32_t workDim;
    uint32_t maxWorkItemsPerWorkGroup;
    size_t workItemsPerWorkGroup[3];
    size_t globalWorkItems[3];
    size_t numWorkGroups[3];

    bool isSipKernelAllocated;
};



