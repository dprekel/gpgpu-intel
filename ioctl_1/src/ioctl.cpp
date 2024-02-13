#include<stdio.h>
#include<string>
#include<iostream>
#include<fcntl.h>
#include<unistd.h>
#include<memory>
#include<vector>
#include<cstring>
#include"NonCopyableClass.h"
#include"HwDeviceId.h"
#include"HwDeviceIdDrm.h"
#include"drm_version.h"
#include<sys/ioctl.h>
#include<linux/limits.h>

void appendHwDeviceId(std::vector<std::unique_ptr<HwDeviceId>> &hwDeviceIds, int fileDescriptor, const char* pciPath);
bool isi915Version(int fileDescriptor);
std::vector<std::unique_ptr<HwDeviceId>> discoverDevices();


int main() {
	using HwDeviceIds = std::vector<std::unique_ptr<HwDeviceId>>;
	
	HwDeviceIds hwDeviceIds = discoverDevices();
	
	uint32_t rootDeviceIndex = 0u;
	
	//for (auto &hwDeviceId : hwDeviceIds) {
	//	if (initHwDeviceIdResources(std::move(hwDeviceId), rootDeviceIndex) == false) {
	//		return -1;
	//	}
	//}
	
	return 0;
}


std::vector<std::unique_ptr<HwDeviceId>> discoverDevices() {
	
	std::vector<std::unique_ptr<HwDeviceId>> hwDeviceIds;
	
	//std::vector<std::string> files = Directory::getFiles(Os::pciDevicesDirectory);
	//TODO: Write a function that reads out the path from the system
	const char* string = "/dev/dri/by-path/pci-0000:00:02.0-render";
	const char* pciPath = "0000:00:02.0";
	
	std::cout << string << std::endl;
	
	int fileDescriptor = open(string, O_RDWR);
	
	std::cout << fileDescriptor << std::endl;
	
	appendHwDeviceId(hwDeviceIds, fileDescriptor, pciPath);
	
	return hwDeviceIds;
}




void appendHwDeviceId(std::vector<std::unique_ptr<HwDeviceId>> &hwDeviceIds, int fileDescriptor, const char* pciPath) {
	if (fileDescriptor >= 0) {
		if (isi915Version(fileDescriptor)) {
			hwDeviceIds.push_back(std::make_unique<HwDeviceIdDrm>(fileDescriptor, pciPath));
		}
		else {
			close(fileDescriptor);
		}
	}
}

bool isi915Version(int fileDescriptor) {
	struct drm_version version = {};
	char name[5] = {};
	version.name = name;
	version.name_len = 5;
	
	std::cout << version.name << std::endl;
	
	int ret = ioctl(fileDescriptor, DRM_IOCTL_VERSION, &version);
	
	std::cout << version.name << std::endl;
	
	if (ret) {
		return false;
	}
	
	name[4] = '\0';
	return strcmp(name, "i915") == 0;
}
