/*
 * Copyright (C) 2020-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include<unistd.h>
#include<fcntl.h>
#include"NonCopyableClass.h"
#include"HwDeviceId.h"
#include"HwDeviceIdDrm.h"

HwDeviceIdDrm::~HwDeviceIdDrm() {
    close(fileDescriptor);
}

