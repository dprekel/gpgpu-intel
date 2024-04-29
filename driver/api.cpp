#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "gpgpu.h"
#include "device.h"
#include "context.h"
#include "kernel.h"
#include "log.h"

#define API_CALL __attribute__((visibility("default")))


pDevice API_CALL* CreateDevice(int* ret) {
    Device* device = new Device();
    int err = device->initialize();
    *ret = err;
    if (err)
        return nullptr;
    pDevice* dev = static_cast<pDevice*>(device);
    return dev;
}


int API_CALL BuildKernel(pDevice* dev, const char* filename, const char* options, int architecture, bool disassemble) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    Kernel* kernel = new Kernel(device, filename, options);
    device->kernel = kernel;
    int ret = kernel->loadProgramSource();
    if (ret)
        return ret;
    ret = kernel->build();
    if (ret)
        return ret;
    ret = kernel->extractMetadata();
    if (ret)
        return ret;
    ret = kernel->createSipKernel();
    if (ret)
        return ret;
    return SUCCESS;
}


int API_CALL CreateContext(pDevice* dev) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    Context* context = new Context(device);
    device->context = context;
    int ret = context->createDrmContext();
    if (device->getNonPersistentContextsSupported()) {
        context->setNonPersistentContext();
    }
    if (ret)
        return CONTEXT_CREATION_FAILED;
    return SUCCESS;
}


int API_CALL CreateBuffer(pDevice* dev, void* buffer, size_t size) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    if (!device->context)
        return NO_CONTEXT_ERROR;
    Context* context = device->context;
    BufferObject* dataBuffer = context->allocateBufferObject(size, 0);
    if (!dataBuffer)
        return BUFFER_ALLOCATION_FAILED;
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    //context->emitPinningRequest(bo);
    buffer = dataBuffer->alloc;
    return SUCCESS;
}


int API_CALL EnqueueNDRangeKernel(pDevice* dev, uint32_t work_dim, const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    if (!device->context)
        return NO_CONTEXT_ERROR;
    Context* context = device->context;
    int ret = context->validateWorkGroups(work_dim, global_work_offset, global_work_size, local_work_size);
    if (ret)
        return ret;
    ret = context->allocateISAMemory();
    if (ret)
        return ret;
    ret = context->createPreemptionAllocation();
    if (ret)
        return ret;
    //ret = context->createIndirectObjectHeap();
    //ret = context->createDynamicStateHeap();
    //ret = context->createSurfaceStateHeap();
    
    ret = context->createCommandBuffer();
    if (ret)
        return ret;
    return SUCCESS;
}


int API_CALL GetInfo(pDevice* dev) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    Log* info = new Log(device);
    info->printDeviceInfo();
    info->printContextInfo();
    return SUCCESS;
}


int API_CALL ReleaseObjects(pDevice* dev) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    delete device->kernel;
    delete device->context;
    delete device;

    return SUCCESS;
}





