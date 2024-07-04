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
    DEBUG_LOG("[DEBUG] Buffer destructor called!\n");
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
    batchBuffer = context->allocateBufferObject(MemoryConstants::pageSize, BufferType::COMMAND_BUFFER);
    if (!batchBuffer)
        return BUFFER_ALLOCATION_FAILED;
    // Program MI_BATCH_BUFFER_END
    auto cmd1 = batchBuffer->ptrOffset<MI_BATCH_BUFFER_END*>(sizeof(MI_BATCH_BUFFER_END));
    *cmd1 = MI_BATCH_BUFFER_END::init();
    // Program NOOP
    auto cmd2 = batchBuffer->ptrOffset<uint32_t*>(sizeof(uint32_t));
    *cmd2 = 0u;

    execBuffer.push_back(dataBuffer.get());
    execBuffer.push_back(batchBuffer.get());

    size_t residencyCount = execBuffer.size();
    execObjects.resize(residencyCount);
    size_t used = sizeof(MI_BATCH_BUFFER_END);
    
    int ret = context->exec(execObjects.data(), execBuffer.data(), residencyCount, used);
    if (ret) {
        execBuffer.clear();
        //TODO: What else must be done here?
        return GEM_EXECBUFFER_FAILED;
    }
    execBuffer.clear();
    //TODO: Further cleanup

    return SUCCESS;
}

