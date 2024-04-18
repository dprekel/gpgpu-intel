#pragma once

class Log {
  public:
    Log(GPU* gpuInfo);
    ~Log();
    void printDeviceInfo();
    void printTopologyInfo();
    const char* decodeCapabilities(uint64_t capability);
    void printContextInfo();
  private:
    GPU* gpuInfo;
};
