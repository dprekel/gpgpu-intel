#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "gpgpu.h"
#include "device.h"
#include "context.h"
#include "kernel.h"
#include "log.h"

#define API_CALL __attribute__((visibility("default")))


int API_CALL CreateDevice(GPU* gpuInfo) {
    int ret;
    Device* device = new Device(gpuInfo);
    ret = device->initializeGPU();
    if (ret) {
        return ret;
    }
    return SUCCESS;
}


int API_CALL CreateContext(GPU* gpuInfo) {
    int ret;
    Context* context = new Context(gpuInfo);
    gpuInfo->context = static_cast<void*>(context);
    ret = context->createDrmContext();
    if (gpuInfo->nonPersistentContextsSupported) {
        context->setNonPersistentContext();
    }
    if (ret) {
        return CONTEXT_CREATION_FAILED;
    }
    return SUCCESS;
}


int API_CALL CreateBuffer(GPU* gpuInfo, void* buffer, size_t size) {
    Context* context = static_cast<Context*>(gpuInfo->context);
    BufferObject* dataBuffer = context->allocateBufferObject(size, 0);
    if (!dataBuffer) {
        return BUFFER_ALLOCATION_FAILED;
    }
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    //context->emitPinningRequest(bo);
    buffer = dataBuffer->alloc;
    return SUCCESS;
}


int API_CALL BuildKernel(GPU* gpuInfo, const char* filename, const char* options) {
    int err;
    Kernel* kernel = new Kernel(gpuInfo, filename, options);
    gpuInfo->kernel = static_cast<void*>(kernel);
    err = kernel->loadProgramSource();
    if (err) {
        return err;
    }
    err = kernel->build();
    if (err) {
        return err;
    }
    err = kernel->extractMetadata();
    if (err) {
        return err;
    }
    return SUCCESS;
}


int API_CALL EnqueueNDRangeKernel(GPU* gpuInfo) {
    int ret;
    Context* context = static_cast<Context*>(gpuInfo->context);
    //ret = context->createPreemptionAllocation();
    //ret = context->createIndirectObjectHeap();
    //ret = context->createDynamicStateHeap();
    //ret = context->createSurfaceStateHeap();
    ret = context->createCommandBuffer();
    return SUCCESS;
}


int API_CALL GetInfo(GPU* gpuInfo) {
    Log* info = new Log(gpuInfo);
    info->printDeviceInfo();
    info->printContextInfo();
    return SUCCESS;
}
