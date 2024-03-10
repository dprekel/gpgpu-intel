#include <stdio.h>

struct HardwareInfo {
    void* x;
    void* y;
};


struct DeviceDescriptor {
    unsigned short deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(HardwareInfo*, bool);
    int eGtType;
    const char* devName;
};

const DeviceDescriptor deviceDescriptorTable[] {
#define NAMEDDEVICE(devId, gt, gtType, devName) {devId, &gt::hwInfo, &gt::setupHardwareInfo, gtType, devName},
#define DEVICE(devId, gt, gtType) {devId, &gt::hwInfo, &gt::setupHardwareInfo, gtType, ""},
#include "devices_base.inl"
#undef DEVICE
#undef NAMEDDEVICE
        {0, nullptr, nullptr, 0}};


int main() {
    unsigned short devId = 6439;
    const DeviceDescriptor* device = nullptr;
    const char* devName = "";
    int eGtType = 0;
    for (auto &d : deviceDescriptorTable) {
        if (devId == d.deviceId) {
            device = &d;
            eGtType = d.eGtType;
            devName = d.devName;
            printf("devName: %s\n", d.devName);
            break;
        }
    }
    return 0;
}
