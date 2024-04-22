#pragma once


enum BufferType {
    BUFFER_HOST_MEMORY,
    KERNEL_ISA,
    KERNEL_ISA_INTERNAL,
    PREEMPTION,
    TAG_BUFFER,
    INTERNAL_HEAP,
    LINEAR_STREAM,
    COMMAND_BUFFER
};

struct BufferObject {
    int bufferType;
    void* alloc;
    size_t size;
    uint32_t handle;
};

class Context {
  public:
    Context(GPU* gpuInfo);
    ~Context();
    int createDrmContext();
    BufferObject* allocateBufferObject(size_t size, uint32_t flags);
    void setNonPersistentContext();
    int validateWorkGroups(uint32_t work_dim, const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size);
    /*
    void generateLocalIDsSimd(void* b, uint16_t* localWorkgroupSize, uint16_t threadsPerWorkGroup, uint8_t* dimensionsOrder, uint32_t simdSize) {
    int createPreemptionAllocation();
    int createDynamicStateHeap();
    int createIndirectObjectHeap();
    int createSurfaceStateHeap();
    */
    int createCommandBuffer();
  private:
    GPU* gpuInfo;
    std::vector<BufferObject*> execBuffer;
    uint32_t vmId;
    uint32_t ctxId;

    uint32_t workDim;
    const size_t globalOffsets[3];      // globalWorkOffset
    const size_t workItems[3];          // region
    const size_t localWorkSizesIn[3];   // localWkgSizeToPass
    const size_t enqueuedWorkSizes[3];  // enqueuedLocalWorkSize
};




