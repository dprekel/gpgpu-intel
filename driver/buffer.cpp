#include <stdint.h>
#include <stdio.h>

#include "buffer.h"
#include "context.h"
#include "gpgpu.h"


Buffer::Buffer(Context* context)
    : context(context) {
}

Buffer::~Buffer() {
    printf("Buffer destructor called!\n");
}

BufferObject* Buffer::getDataBuffer() {
    return this->dataBuffer.get();
}

int Buffer::allocateAndPinDataBuffer(size_t size) {
    dataBuffer = context->allocateBufferObject(size);
    if (!dataBuffer)
        return BUFFER_ALLOCATION_FAILED;
    mem = dataBuffer->cpuAddress;
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    
    execObject = {0};
    BufferObject* dataBufferPtr = dataBuffer.get();
    int ret = context->exec(&execObject, &dataBufferPtr, 1, 4);
    if (ret)
        return GEM_EXECBUFFER_FAILED;

    return SUCCESS;
}

