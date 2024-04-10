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
    void* alignedMalloc(size_t size);
    BufferObject* allocUserptr(int fd, uintptr_t alloc, size_t size, uint32_t flags);
    void enqueueBufferObject(BufferObject* bo);
    void setNonPersistentContext();
    void generateLocalIDsSimd(void* b, uint16_t* localWorkgroupSize, uint16_t threadsPerWorkGroup, uint8_t* dimensionsOrder, uint32_t simdSize) {
    int createPreemptionAllocation();
    int createDynamicStateHeap();
    int createIndirectObjectHeap();
    int createSurfaceStateHeap();
  private:
    GPU* gpuInfo;
    std::vector<BufferObject*> execBuffer;
    uint32_t vmId;
    uint32_t ctxId;
};




