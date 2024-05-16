#pragma once

#include <stdio.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include "device.h"
#include "gpgpu.h"
#include "kernel.h"


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
    int bufferType;
    void* alloc;
    size_t size;
    uint32_t handle;
};

class Device;
class Kernel;

class Context : public pContext {
  public:
    Context(Device* device);
    ~Context();
    int createDrmContext();
    BufferObject* allocateBufferObject(size_t size, uint32_t flags);
    void setNonPersistentContext();
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size);
    /*
    void generateLocalIDsSimd(void* b, uint16_t* localWorkgroupSize, uint16_t threadsPerWorkGroup, uint8_t* dimensionsOrder, uint32_t simdSize) {
    int createDynamicStateHeap();
    int createIndirectObjectHeap();
    */
    int createSurfaceStateHeap();
    int allocateISAMemory();
    int createScratchAllocation();
    int createPreemptionAllocation();
    int createCommandBuffer();
    Device* device;
    Kernel* kernel = nullptr;
  private:
    const HardwareInfo* hwInfo = nullptr;
    KernelFromPatchtokens* kernelData = nullptr;
    std::vector<std::unique_ptr<BufferObject>> execBuffer;
    uint32_t vmId;
    uint32_t ctxId;

    uint32_t workDim;
    size_t globalOffsets[3];      // globalWorkOffset
    size_t workItems[3];          // region
    size_t localWorkSizesIn[3];   // localWkgSizeToPass
    size_t enqueuedWorkSizes[3];  // enqueuedLocalWorkSize
};



