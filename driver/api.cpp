#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "gpgpu.h"
#include "device.h"
#include "context.h"
#include "kernel.h"
#include "log.h"

#define API_CALL __attribute__((visibility("default")))


pDevice* API_CALL CreateDevice(int* ret) {
    Device* device = new Device();
    int err = device->initializeGPU();
    *ret = err;
    if (err) {
        return nullptr;
    }
    pDevice* pdevice = static_cast<pDevice*>(device);
    return pdevice;
}


int API_CALL CreateContext(pDevice* device) {
    if (!device) {
        return NO_DEVICE;
    }
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


int API_CALL CreateBuffer(pDevice* device, void* buffer, size_t size) {
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


int API_CALL BuildKernel(pDevice* device, const char* filename, const char* options, int architecture, bool disassemble) {
    Kernel* kernel = new Kernel(gpuInfo, filename, options);
    gpuInfo->kernel = static_cast<void*>(kernel);
    int ret = kernel->loadProgramSource();
    if (ret) {
        return ret;
    }
    ret = kernel->build();
    if (ret) {
        return ret;
    }
    ret = kernel->extractMetadata();
    if (ret) {
        return ret;
    }
    ret = kernel->createSipKernel();
    if (ret) {
        return ret;
    }
    return SUCCESS;
}


int API_CALL EnqueueNDRangeKernel(pDevice* device, uint32_t work_dim, const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size) {
    Context* context = static_cast<Context*>(gpuInfo->context);
    int ret = context->validateWorkGroups(work_dim, global_work_offset, global_work_size, local_work_size);
    if (ret) {
        return ret;
    }
    ret = context->allocateISAMemory();
    if (ret) {
        return ret;
    }
    //ret = context->createPreemptionAllocation();
    //ret = context->createIndirectObjectHeap();
    //ret = context->createDynamicStateHeap();
    //ret = context->createSurfaceStateHeap();
    
    ret = context->createCommandBuffer();
    if (ret) {
        return ret;
    }
    return SUCCESS;
}


int API_CALL GetInfo(pDevice* device) {
    Log* info = new Log(gpuInfo);
    info->printDeviceInfo();
    info->printContextInfo();
    return SUCCESS;
}


int API_CALL ReleaseObjects(pDevice* device) {
    delete static_cast<Context*>(gpuInfo->context);
    delete static_cast<Kernel*>(gpuInfo->kernel);
    delete gpuInfo;

    return SUCCESS;
}


