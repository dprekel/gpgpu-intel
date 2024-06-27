#pragma once

#include <stdint.h>

#include <memory>

#include "context.h"

//TODO: Check if kernel arguments are set properly if EnqueueNDRangeKernel is called

class Buffer : public pBuffer {
  public:
    Buffer(Context* context);
    ~Buffer();
    BufferObject* getDataBuffer();
    int allocateAndPinDataBuffer(size_t size);

    Context* context = nullptr;
  private:
    std::unique_ptr<BufferObject> dataBuffer;
    drm_i915_gem_exec_object2 execObject;
};

