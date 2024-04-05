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
    void* alignedMalloc(size_t size);
    int allocUserptr(uintptr_t alloc, size_t size, uint32_t flags);
  private:
    std::vector<BufferObject> execBuffer;
}
