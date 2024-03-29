# GPGPU-Intel
When finished, this will be a very simple userspace driver for general purpose computation on Intel Integrated Graphics GPUs. It is based on reverse engineering Intels [NEO Compute Runtime](https://github.com/intel/compute-runtime) for OpenCL and extracts its core functionality. While Intels driver is about 200000 lines of C++, this will be around 2000 lines.

## Dependencies
Make sure you are using a Linux distribution on a machine with Intel Core CPU (with integrated graphics) and i915 kernel driver. 

This driver links against the Intel Graphics Compiler ([IGC](https://github.com/intel/intel-graphics-compiler)). On Ubuntu it can be installed with
```sh
$ sudo apt install libigc-dev libigc-tools libigc1 libigdfcl-dev libigdfcl1
```

## Installation
This driver can be installed with
```sh
$ cd driver
$ make
$ sudo make install
```
This creates a shared library and installs it into `/usr/local/lib`. Before linking any application against it, you may have to set your `LD_LIBRARY_PATH`:
```sh
export "$LD_LIBRARY_PATH:/usr/local"
```

