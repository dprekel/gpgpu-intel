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


int main() {
	std::vector<std::unique_ptr<HwDeviceId>> hwDeviceIds;
	//TODO: Write a function that reads out the path from the system
	const char* string = "/dev/dri/by-path/pci-0000:00:02.0-render";
	const char* pciPath = "0000:00:02.0";
	//printf("%s\n", string);
	std::cout << string << std::endl;
	
	int fileDescriptor = open(string, O_RDWR);	
	
	std::cout << fileDescriptor << std::endl;

	appendHwDeviceId(hwDeviceIds, fileDescriptor, pciPath);

	return 0;
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
