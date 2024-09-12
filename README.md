# GPGPU-Intel
## Overview
gpgpu-intel is a minimal userspace driver for general purpose computation on Intel GPUs.
It is based on reverse engineering Intels [NEO Compute Runtime](https://github.com/intel/compute-runtime) for OpenCL and extracts its core functionality. 
While Intels driver is about 200000 lines of C++, this driver has 6500 lines. Its core features include:
- Immediate execution of arbitrary compute kernels
- Support for all address space qualifiers (global, constant, local, private)
- Support for memory barriers
- Save compiled kernel binaries into file

Not included are:
- Event-based kernel execution
- Kernel arguments of the following type: image, sampler, command queue

## Build Instructions
Building and running this driver requires
- Intel Processor with integrated GEN8, GEN9 or GEN11 GPU (GEN12 and Xe not yet supported)
- Linux distribution of your choice

This driver links against the [Intel Graphics Compiler (IGC)](https://github.com/intel/intel-graphics-compiler). On Ubuntu it can be installed with
```sh
sudo apt install libigc-dev libigc1 libigdfcl-dev libigdfcl1
```
Alternatively, there are release builds available [here](https://github.com/intel/intel-graphics-compiler/releases).
Compile this driver by doing the following:
```sh
git clone https://github.com/dprekel/gpgpu-intel.git
cd gpgpu-intel/driver
make DEBUG=1 INFO=1
sudo make install
```
This creates a shared library `libigpgpu.so` and installs it in `/usr/local/lib`. `DEBUG=1` compiles with debug symbols and console logging, `INFO=1` will print device info to the console when running an application.

## Documentation
The API documentation can be found [here](https://dprekel.github.io/html/gpgpu__api_8h.html) (not complete yet).


## Driver Internals
The image below is a rather straightforward description about the GPU driver stack: Arbitrary applications call into the userspace GPU driver via an OpenCL-like API. The driver initializes the device, creates a DRM (Direct Rendering Manager) context and compiles the kernel code into GPU executable instructions. The compiler links against Clang/LLVM at runtime.

<img src="docs/GPU_Driver_Stack.png?raw=true" alt="GPU driver stack" width="570" />

The userspace driver then constructs a whole lot of buffer objects (kernel data, kernel instructions, GPU batchbuffers, scratch space, preemption, ...). 
Pointers to these objects are passed to the Linux kernelspace driver (i915) via the DRM\_IOCTL\_I915\_GEM\_EXECOBJECT2 IOCTL. 
i915 pins all buffer objects into the per-process graphics translation table (ppGTT). 
The batchbuffer is then copied into a ring buffer by i915 so that the Command Streamer Engine of the GPU can directly access it by direct memory access (DMA). 
The command streamer then executes the commands one by one. 
The pointers to the other buffer objects which are stored in the batchbuffer tell the command streamer where to find all needed data. 
I created an image to show this in detail (for a matmul example):

![](docs/GPU_Driver_Flowchart.png?raw=true)

## Examples
Example applications that use the API can be found [here](test).
The following example shows the [calculation of nonbonded interactions](test/gpgpu-particle-mesh-ewald) in molecular dynamics simulations:

<img src="docs/Particle_Mesh_Ewald.gif" width="510" height="600" />

## ToDo
- Add support for GEN12 and Xe Graphics
- Once an application has issued a couple of dozen kernel executions (depends on kernel), some buffer objects will overflow, needs to be fixed
- There is an issue with local memory when running kernels in a loop
- For each batchbuffer submission, all GPU commands are executed (the first few commands are only needed once for GPU setup)

## Links
[Intel Graphics Hardware documentation](https://www.intel.com/content/www/us/en/docs/graphics-for-linux/developer-reference/1-0/overview.html)

[WikiChip about GEN9 hardware](https://en.wikichip.org/wiki/intel/microarchitectures/gen9.5)

[Debug a GPU kernel with gdb](https://www.intel.com/content/www/us/en/docs/distribution-for-gdb/get-started-guide-linux/2023-0/overview.html)

[How to build a simple GPU in Verilog](https://github.com/adam-maj/tiny-gpu)



