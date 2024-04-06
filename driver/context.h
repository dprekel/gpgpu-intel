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
    int allocUserptr(int fd, uintptr_t alloc, size_t size, uint32_t flags);
  private:
    GPU* gpuInfo;
    std::vector<BufferObject> execBuffer;
    uint32_t vmId;
    uint32_t ctxId;
};
