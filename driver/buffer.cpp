#include <stdint.h>
#include <stdio.h>

#include "buffer.h"
#include "commands_gen9.h"
#include "context.h"
#include "gpgpu.h"


Buffer::Buffer(Context* context)
    : context(context) {
}

Buffer::~Buffer() {
    DBG_LOG("[DEBUG] Buffer destructor called!\n");
}

BufferObject* Buffer::getDataBuffer() {
    return dataBuffer.get();
}

int Buffer::allocateAndPinDataBuffer(size_t size) {
    //TODO: Do not forget alignUp
    dataBuffer = context->allocateBufferObject(size, BufferType::BUFFER_HOST_MEMORY);
    if (!dataBuffer)
        return BUFFER_ALLOCATION_FAILED;
    mem = dataBuffer->cpuAddress;

    execBuffer.push_back(dataBuffer.get());
    execBuffer.push_back(context->getBatchBuffer());

    size_t residencyCount = execBuffer.size();
    execObjects.resize(residencyCount);
    size_t batchSize = sizeof(MI_BATCH_BUFFER_END);
    
    int ret = context->exec(execObjects.data(), execBuffer.data(), residencyCount, batchSize, 0u);
    if (ret) {
        execBuffer.clear();
        //TODO: What else must be done here?
        return GEM_EXECBUFFER_FAILED;
    }
    execBuffer.clear();
    //TODO: Further cleanup

    return SUCCESS;
}

