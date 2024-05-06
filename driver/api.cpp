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


pContext API_CALL* CreateContext(pDevice* dev, int* ret) {
    *ret = 0;
    if (!dev) {
        *ret = NO_DEVICE_ERROR;
        return nullptr;
    }
    Device* device = static_cast<Device*>(dev);
    if (device->context) {
        return static_cast<pContext*>(device->context);
    }
    Context* context = new Context(device);
    device->context = context;
    *ret = context->createDrmContext();
    if (*ret) {
        return nullptr;
    }
    if (device->getNonPersistentContextsSupported()) {
        context->setNonPersistentContext();
    }
    pContext* cont = static_cast<pContext*>(context);
    return cont;
}


pKernel API_CALL* BuildKernel(pContext* cont,
                        const char* filename,
                        const char* options,
                        uint16_t chipset_id,
                        bool enableDisassemble,
                        int* ret) {
    *ret = 0;
    if (!cont) {
        *ret = NO_CONTEXT_ERROR;
        return nullptr;
    }
    *ret = 0;
    Context* context = static_cast<Context*>(cont);
    Kernel* kernel = new Kernel(context, filename, options);
    *ret = kernel->initialize();
    if (*ret)
        return nullptr;
    *ret = kernel->build(chipset_id);
    if (*ret)
        return nullptr;
    if (enableDisassemble) {
        *ret = kernel->disassembleBinary();
        if (*ret)
            return nullptr;
    }
    if (!chipset_id) {
        *ret = kernel->extractMetadata();
        if (*ret)
            return nullptr;
        *ret = kernel->createSipKernel();
        if (*ret)
            return nullptr;
    }
    pKernel* kern = static_cast<pKernel*>(kernel);
    return kern;
}


int API_CALL CreateBuffer(pContext* cont,
                        void* buffer,
                        size_t size) {
    if (!cont)
        return NO_CONTEXT_ERROR;
    Context* context = static_cast<Context*>(cont);
    BufferObject* dataBuffer = context->allocateBufferObject(size, 0);
    if (!dataBuffer)
        return BUFFER_ALLOCATION_FAILED;
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    //context->emitPinningRequest(bo);
    buffer = dataBuffer->alloc;
    return SUCCESS;
}


int API_CALL EnqueueNDRangeKernel(pContext* cont,
                        pKernel* kern,
                        uint32_t work_dim,
                        const size_t* global_work_offset,
                        const size_t* global_work_size,
                        const size_t* local_work_size) {
    if (!cont)
        return NO_CONTEXT_ERROR;
    Context* context = static_cast<Context*>(cont);
    context->kernel = static_cast<Kernel*>(kern);
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
    context->kernel = nullptr;
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


int API_CALL ReleaseDevice(pDevice* dev) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    delete device;
    return SUCCESS;
}


int API_CALL ReleaseContext(pContext* cont) {
    if (!cont)
        return NO_CONTEXT_ERROR;
    Context* context = static_cast<Context*>(cont);
    delete context;
    return SUCCESS;
}


int API_CALL ReleaseKernel(pKernel* kern) {
    if (!kern)
        return NO_KERNEL_ERROR;
    Kernel* kernel = static_cast<Kernel*>(kern);
    delete kernel;
    return SUCCESS;
}






