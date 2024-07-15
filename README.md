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



When using OpenCL, there is no way to dump your device kernel into a file to inspect/disassemble it. You have to run your OpenCL host program within the Intercept Layer for OpenCL. Here, you just specify the enableBinaryDump field of the BuildKernel API as true.

## Dependencies
Different to Intels NEO OpenCL driver, this driver doesn't need interface headers of the IGC to build successfully. This poises a risk of compatibility issues with specific IGC builds. 
