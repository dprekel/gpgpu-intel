#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <vector>

#include "commandsGen9.h"
#include "context.h"
#include "gpgpu_api.h"
#include "ioctl.h"
#include "utils.h"


CommandDispatcherXe2::CommandDispatcherXe2(Device* device) 
        : device(device),
          hwInfo(device->getDeviceDescriptor()->pHwInfo),
          workItemsPerWorkGroup{1, 1, 1},
          globalWorkItems{1, 1, 1},
          numWorkGroups{1, 1, 1},
          isMidThreadLevelPreemptionSupported(device->getMidThreadPreemptionSupport()) {
    this->setMaxWorkGroupSize();
    this->setMaxThreadsForVfe();
}

CommandDispatcherXe2::~CommandDispatcherXe2() {
}

std::unique_ptr<BufferObject> CommandDispatcherXe2::allocateBufferObject(size_t size, int bufferType) {
    drm_xe_gem_create create = {0};
    create.size = size;
    create.placement = ;
    create.cpu_caching = ;
    int ret = ioctl(device->fd, DRM_IOCTL_XE_GEM_CREATE, &create);
    if (ret)
        return nullptr;
    
    GemMmapOffset mmapOffset = {0};
    mmapOffset.handle = bo->handle;
    mmapOffset.flags = 4;
    int ret = ioctl(device->fd, DRM_IOCTL_XE_GEM_MMAP_OFFSET, &mmapOffset);
    if (ret) {
        mmapOffset.flags = flags;
        ret = ioctl(device->fd, DRM_IOCTL_XE_GEM_MMAP_OFFSET);
    }
    if (!ret) {
        bo->offset = mmapOffset.offset;
    } else {
        return nullptr;
    }
    void* addr = mmap(nullptr, bo->size, PROT_WRITE | PROT_READ, MAP_SHARED, device->fd, static_cast<off_t>(bo->offset));
    if (addr == MAP_FAILED)
        return nullptr;
    bo->cpuAddress = addr;

}

bool CommandDispatcherXe2::bindBufferObject(BufferObject* bo) {
    // maybe lock a mutex here
    drm_xe_sync sync = {0};
    sync.type = DRM_XE_SYNC_TYPE_USER_FENCE;
    sync.flags = DRM_XE_SYNC_FLAG_SIGNAL;
    sync.addr = userFence->addr;
    sync.timeline_value = userFence->value;

    drm_xe_vm_bind bind = {0};
    bind.vm_id = this->vmId;
    bind.num_binds = 1;
    bind.bind.range = length;
    bind.bind.obj_offset = offset;
    bind.bind.pat_index = static_cast<uint16_t>(patIndex);
    bind.bind.extensions = extensions;
    bind.bind.flags = static_cast<uint32_t>(flags);
    bind.bind.addr = start; // or 0
    bind.num_syncs = 1;
    bind.syncs = reinterpret_cast<uintptr_t>(&sync);
    if (isBind) {
        bind.bind.op = DRM_XE_VM_BIND_OP_MAP;
        bind.bind.obj = bo->handle;
        if (userptr) {
            bind.bind.op = DRM_XE_VM_BIND_OP_MAP_USERPTR;
            bind.bind.obj = 0;
            bind.bind.obj_offset = userptr;
        }
    } else {
        if (sharedSystemUsmEnabled) {
            bind.bind.op = DRM_XE_BIND_OP_MAP;
            bind.bind.flags |= DRM_XE_VM_BIND_FLAG_SYSTEM_ALLOCATOR;
        } else {
            bind.bind.op = DRM_XE_VM_BIND_OP_UNMAP;
            if (userptr) {
                bind.bind.obj_offset = userptr;
            }
        }
        bind.bind.obj = 0;
    }
    int ret = ioctl(device->fd, DRM_IOCTL_XE_VM_BIND, &bind);
    if (ret)
        return false;
    
    drm_xe_wait_user_fence waitUserFence = {0};
    waitUserFence.addr = sync.addr;
    waitUserFence.op = DRM_XE_UFENCE_WAIT_OP_EQ;
    waitUserFence.value = sync.timeline_value;
    waitUserFence.mask = std::numeric_limits<uint64_t>::max();
    waitUserFence.timeout = 1000000000ll;
    waitUserFence.exec_queue_id = bind.exec_queue_id;
    ret = ioctl(device->fd, DRM_IOCTL_XE_WAIT_USER_FENCE, &waitUserFence);
    if (ret)
        return false;
    return true;
}


int CommandDispatcherXe2::createCommandStreamReceiver() {
    if (!this->commandStreamCSR) {
        this->commandStreamCSR = this->allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::COMMAND_BUFFER);
        if (this->commandStreamCSR)
            return BUFFER_ALLOCATION_FAILED;
    }
    this->commandStreamCSR->currentTaskOffset = 0u;

    // Program Memory Fences
    if (this->globalFenceAllocation) {
        auto cmd1 = this->commandStreamCSR->ptrOffset<STATE_SYSTEM_MEM_FENCE_ADDRESS*>(sizeof(STATE_SYSTEM_MEM_FENCE_ADDRESS));
        *cmd1 = STATE_SYSTEM_MEM_FENCE_ADDRESS::init();
        cmd1->Bitfield.SystemMemoryFenceAddress = this->globalFenceAllocation->gpuAddress >> 0xc;
    }

    // Program EU Thread policy
    auto cmd2 = this->commandStreamCSR->ptrOffset<STATE_COMPUTE_MODE*>(sizeof(STATE_COMPUTE_MODE));
    *cmd2 = STATE_COMPUTE_MODE::init();
    cmd2->Bitfield.MemoryAllocationForScratchAndMidthreadPreemptionBuffers = true;
    cmd2->Bitfield.

    // Program Preemption
    auto cmd3 = this->commandStreamCSR->ptrOffset<STATE_CONTEXT_DATA_BASE_ADDRESS*>(sizeof(STATE_CONTEXT_DATA_BASE_ADDRESS));
    *cmd3 = STATE_CONTEXT_DATA_BASE_ADDRESS::init();
    cmd3->Bitfield.ContextDataBaseAddress = this->preemptionAllocation->gpuAddress >> 0xc;

    // Program VFE State
}

/* Buffers in Xe2 matmul execbuffer:

COMMAND_BUFFER_1
INTERNAL_HEAP_1
COMMAND_BUFFER_2
COMMAND_BUFFER_3
TIMESTAMP_PACKET_TAG_BUFFER
CONSTANT_SURFACE
KERNEL_ISA_1
BUFFER_1
BUFFER_2
BUFFER_3
KERNEL_ISA_2
SCRATCH_SURFACE
LINEAR_STREAM_1
LINEAR_STREAM_2
INTERNAL_HEAP_2
GLOBAL_FENCE
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER_4
COMMAND_BUFFER_5
*/
