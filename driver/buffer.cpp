#include <stdint.h>
#include <stdio.h>

#include "buffer.h"
#include "context.h"
#include "gpgpu.h"


Buffer::Buffer(Context* context)
    : context(context) {
}

Buffer::~Buffer() {
    printf("Context destructor called!\n");
}

int Buffer::allocateAndPinDataBuffer(size_t size) {
    dataBuffer = context->allocateBufferObject(size);
    if (!dataBuffer)
        return BUFFER_ALLOCATION_FAILED;
    mem = dataBuffer->cpuAddress;
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    
    execObject = {0};
    int ret = context->exec(&execObject, dataBuffer.get(), 1);
    if (ret)
        return GEM_EXECBUFFER_FAILED;

    return SUCCESS;
}

