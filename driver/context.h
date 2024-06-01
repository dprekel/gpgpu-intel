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


class Context : public pContext {
  public:
    Context(Device* device);
    ~Context();
    void setKernelData(KernelFromPatchtokens* kernelData);
    KernelFromPatchtokens* getKernelData();
    void setMaxWorkGroupSize();
    int createDrmContext();
    void setNonPersistentContext();
    BufferObject* allocateBufferObject(size_t size, uint32_t flags);
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size);
    int createSurfaceStateHeap();
    int createIndirectObjectHeap();
    int createDynamicStateHeap();
    int allocateISAMemory();
    int createScratchAllocation();
    int createPreemptionAllocation();
    int createCommandBuffer();

    Device* device;
    Kernel* kernel = nullptr;
  private:
    void generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize);

    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;
    std::vector<std::unique_ptr<BufferObject>> execBuffer;
    uint32_t vmId;
    uint32_t ctxId;

    uint32_t workDim;
    uint32_t maxWorkItemsPerWorkGroup;
    size_t workItemsPerWorkGroup[3];
    size_t globalWorkItems[3];
};



