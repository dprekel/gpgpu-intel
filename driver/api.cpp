#include <stdio.h>
#include <stdint.h>

#include <vector>

#include "buffer.h"
#include "context.h"
#include "device.h"
#include "gpgpu.h"
#include "kernel.h"

#define API_CALL __attribute__((visibility("default")))


API_CALL std::vector<pDevice*> CreateDevices(int* ret) {
    *ret = 0;
    std::vector<pDevice*> devices;
    std::vector<int> devIDs = openDevices(ret);
    if (*ret || devIDs.size() == 0)
        return devices;
    CompilerInfo compilerInfo = initCompiler(ret);
    if (*ret)
        return devices;
    for (auto& ID : devIDs) {
        Device* device = new Device(ID, &compilerInfo);
        *ret = device->initialize();
        if (*ret)
            return devices;     // problem: information loss if previous return value is overwritten
        pDevice* dev = static_cast<pDevice*>(device);
        devices.push_back(dev);
    }
    return devices;
}


API_CALL pContext* CreateContext(pDevice* dev, int* ret) {
    *ret = 0;
    if (!dev) {
        *ret = NO_DEVICE_ERROR;
        return nullptr;
    }
    Device* device = static_cast<Device*>(dev);
    if (device->context) {
        *ret = CONTEXT_ALREADY_EXISTS;
        return nullptr;
    }
    Context* context = new Context(device);
    device->context = context;
    *ret = context->createDRMContext();
    if (*ret)
        return nullptr;
    *ret = context->createTagAllocation();
    if (*ret)
        return nullptr;
    *ret = context->createPreemptionAllocation();
    if (*ret)
        return nullptr;
    pContext* cont = static_cast<pContext*>(context);
    return cont;
}


API_CALL pKernel* BuildKernel(pContext* cont,
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
        if (!context->getIsSipKernelAllocated()) {
            *ret = kernel->retrieveSystemRoutineInstructions();
            if (*ret)
                return nullptr;
        }
    // Set kernelBuildSuccessful to true
    }
    pKernel* kern = static_cast<pKernel*>(kernel);
    return kern;
}


API_CALL pBuffer* CreateBuffer(pContext* cont,
                        size_t size,
                        int* ret) {
    *ret = 0;
    if (!cont) {
        *ret = NO_CONTEXT_ERROR;
        return nullptr;
    }
    Context* context = static_cast<Context*>(cont);
    Buffer* bufferObj = new Buffer(context);
    *ret = bufferObj->allocateAndPinDataBuffer(size);
    if (*ret)
        return nullptr;
    pBuffer* buf = static_cast<pBuffer*>(bufferObj);
    return buf;
}


API_CALL int SetKernelArg(pKernel* kern,
                        uint32_t arg_index,
                        size_t arg_size,
                        void* arg_value) {
    Kernel* kernel = static_cast<Kernel*>(kern);
    if (!kernel)
        return INVALID_KERNEL;
    int ret = kernel->setArgument(arg_index, arg_size, arg_value);
    if (ret)
        return ret;
    return SUCCESS;
}


API_CALL int EnqueueNDRangeKernel(pContext* cont,
                        pKernel* kern,
                        uint32_t work_dim,
                        const size_t* global_work_size,
                        const size_t* local_work_size) {
    if (!cont)
        return NO_CONTEXT_ERROR;
    Context* context = static_cast<Context*>(cont);
    if (!kern || !context->getKernelData())
        return INVALID_KERNEL;
    context->kernel = static_cast<Kernel*>(kern);
    int ret = context->validateWorkGroups(work_dim, global_work_size, local_work_size);
    if (ret)
        return ret;
    ret = context->createGPUAllocations();
    if (ret)
        return ret;
    ret = context->populateAndSubmitExecBuffer();
    if (ret)
        return ret;
    ret = context->finishExecution();
    if (ret)
        return ret;
    context->kernel = nullptr; //TODO: maybe this is not good
    return SUCCESS;
}


API_CALL int ReleaseDevice(pDevice* dev) {
    if (!dev)
        return NO_DEVICE_ERROR;
    Device* device = static_cast<Device*>(dev);
    delete device;
    return SUCCESS;
}


API_CALL int ReleaseContext(pContext* cont) {
    if (!cont)
        return NO_CONTEXT_ERROR;
    Context* context = static_cast<Context*>(cont);
    delete context;
    return SUCCESS;
}


API_CALL int ReleaseKernel(pKernel* kern) {
    if (!kern)
        return NO_KERNEL_ERROR;
    Kernel* kernel = static_cast<Kernel*>(kern);
    delete kernel;
    return SUCCESS;
}


API_CALL int ReleaseBuffer(pBuffer* buf) {
    if (!buf)
        return NO_BUFFER_ERROR;
    Buffer* buffer = static_cast<Buffer*>(buf);
    delete buffer;
    return SUCCESS;
}




