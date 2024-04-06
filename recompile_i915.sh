#!/bin/bash

if [ -d /home/david/module_test/lib ]; then
    rm -r /home/david/module_test/lib
fi
cd /home/david/Dropbox/linux/linux-hwe-5.19-5.19.0
make -j4 M=drivers/gpu/drm/i915 modules || exit 1
make M=drivers/gpu/drm/i915 INSTALL_MOD_STRIP=1 INSTALL_MOD_PATH=/home/david/module_test modules_install
depmod -A
sudo rm /lib/modules/5.19.0-50-generic/kernel/drivers/gpu/drm/i915/i915.ko
sudo cp /home/david/module_test/lib/modules/5.19.17-50-generic/extra/i915.ko /lib/modules/5.19.0-50-generic/kernel/drivers/gpu/drm/i915/
