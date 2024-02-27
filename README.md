# GPGPU-Intel
A minimal userspace driver for general purpose computation on Intel Integrated Graphics GPUs. Intels [NEO compute runtime](https://github.com/intel/compute-runtime) for OpenCL is a large project. To learn about the inner workings of these drivers, we can extract the core functionality and write our own driver.

## Reimplementing clCreateBuffer

