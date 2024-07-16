#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include "context.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "ioctl.h"
#include "kernel.h"

#ifdef INFO
#define INFO_LOG(...) printf(__VA_ARGS__)
#else
#define INFO_LOG(...) do {} while (0)
#endif
#ifdef DEBUG
#define DBG_LOG(...) printf(__VA_ARGS__)
#else
#define DBG_LOG(...) do {} while (0)
#endif

class Context;
class Kernel;

struct DeviceDescriptor {
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    const char* devName;
};

struct CompilerInfo {
    const char* igcName = "libigc.so.1";
    const char* fclName = "libigdfcl.so.1";
    CIFMain* igcMain = nullptr;
    CIFMain* fclMain = nullptr;
};


class Device : public pDevice {
  public:
    Device(int fd, CompilerInfo* compilerInfo);
    ~Device();
    static std::vector<int> openDevices(int* err);
    static CompilerInfo initCompiler(int* ret);
    int initialize();
    std::unique_ptr<DeviceDescriptor> getDeviceInfoFromDescriptorTable(uint16_t deviceID);
    CIFMain* getIgcMain();
    CIFMain* getFclMain();
    DeviceDescriptor* getDeviceDescriptor();
    std::string& getDeviceExtensions();
    bool getMidThreadPreemptionSupport();

    Context* context = nullptr;
    int fd = 0;
    uint64_t gpuBaseAddress = 0u;
  private:
    bool checkDriverVersion();
    void getDeviceInfoFromHardwareConfigBlob();
    void setEdramSize(SystemInfo* sysInfo);
    void setLastLevelCacheSize(SystemInfo* sysInfo);
    void setDeviceExtensions(FeatureTable* featureTable);
    int checkPreemptionSupport(FeatureTable* featureTable);
    int retrieveTopologyInfo(SystemInfo* sysInfo);
    int calculateGraphicsBaseAddress();
    int getParamIoctl(int param, int* paramValue);
    std::unique_ptr<uint8_t[]> queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length);
    
    CIFMain* igcMain = nullptr;
    CIFMain* fclMain = nullptr;
    std::unique_ptr<DeviceDescriptor> deviceDescriptor;
    std::string deviceExtensions;
    uint32_t numDevices = 0u;
    char driver_name[5];
    int deviceID = 0;
    int revisionID = 0;
    uint16_t subSliceCountPerSlice = 0u;
    uint16_t euCountPerSubSlice = 0u;
    bool isHardwareConfigBlobSupported = false;
    bool isMidThreadLevelPreemptionSupported = false;
};





