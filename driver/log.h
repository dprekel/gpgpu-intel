#pragma once

#include <stdint.h>

#include "gpgpu.h"

class Log {
  public:
    Log(Device* device);
    ~Log();
    void printDeviceInfo();
    void printTopologyInfo();
    const char* decodeCapabilities(uint64_t capability);
    void printContextInfo();
  private:
    Device* device;
};
