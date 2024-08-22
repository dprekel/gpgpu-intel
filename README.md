# GPGPU-Intel
When finished, this will be a very simple userspace driver for general purpose computation on Intel Integrated Graphics GPUs. It is based on reverse engineering Intels [NEO Compute Runtime](https://github.com/intel/compute-runtime) for OpenCL and extracts its core functionality. While Intels driver is about 200000 lines of C++, this will be around 6000 lines.


## Installation
Building and running this driver requires
- Intel Processor with integrated GEN8, GEN9 or GEN11 GPU (GEN12 and Xe/Xe2 not yet supported)
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
The API documentation can be read [here](docs/html/gpgpu__api_8h.html).


## Driver Internals
![](docs/GPU_Driver_Flowchart.png?raw=true)
The batchbuffer is then put into a ring buffer by the kernel driver so that the Command Streamer Engine of the GPU can directly access it by DMA. The Command Streamer then executes the commands one by one.

## Trash
When using OpenCL, there is no way to dump your device kernel into a file to inspect/disassemble it. You have to run your OpenCL host program within the Intercept Layer for OpenCL. Here, you just specify the enableBinaryDump field of the BuildKernel API as true.

Different to Intels NEO OpenCL driver, this driver doesn't need interface headers of the IGC to build successfully. This poises a risk of compatibility issues with specific IGC builds. 

