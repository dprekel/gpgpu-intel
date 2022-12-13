#include<string>

class HwDeviceIdDrm : public HwDeviceId {
  public:
    static constexpr DriverModelType driverModelType = DriverModelType::DRM;

    HwDeviceIdDrm(int fileDescriptorIn, const char *pciPathIn)
        : HwDeviceId(DriverModelType::DRM),
          fileDescriptor(fileDescriptorIn), pciPath(pciPathIn) {}
    ~HwDeviceIdDrm() override;
    int getFileDescriptor() const { 
        return fileDescriptor; 
    }
    const char *getPciPath() const { 
        return pciPath.c_str(); 
    }

  protected:
    const int fileDescriptor;
    const std::string pciPath;
};

