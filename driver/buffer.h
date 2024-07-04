#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "context.h"


class Buffer : public pBuffer {
  public:
    Buffer(Context* context);
    ~Buffer();
    BufferObject* getDataBuffer();
    int allocateAndPinDataBuffer(size_t size);

    Context* context = nullptr;
  private:
    std::unique_ptr<BufferObject> dataBuffer;
    std::unique_ptr<BufferObject> batchBuffer;

    std::vector<BufferObject*> execBuffer;
    std::vector<drm_i915_gem_exec_object2> execObjects;
};

