# Using GROMACS with Intel Integrated Graphics GPUs

## Compiling GROMACS
On Intel machines, GPU-accelerated GROMACS can be compiled with:
```sh
$ tar xfz gromacs-2024.1.tar.gz
$ cd gromacs-2024.1
$ mkdir build
$ cmake .. -DGMX_BUILD_OWN_FFTW=ON -DGMX_GPU=OpenCL -DGMX_GPU_NB_CLUSTER_SIZE=4 -DCMAKE_BUILD_TYPE=Debug -DREGRESSIONTEST_DOWNLOAD=ON
$ make -j4
$ make check
$ sudo make install
$ source /usr/local/gromacs/bin/GMXRC
```

## Running Molecular Dynamics Test Simulations
Used the Lysozyme tutorial to run test simulations. The NVT equilibration simulation (100 ps, 2 fs timestep, 50000 steps in total) was carried out on two systems:

- Intel Core i5-6267U (3.3GHz) with Intel Iris Graphics 550; 8 GB RAM
- Apple Macbook Air M1 with 8-Core-GPU; 8 GB RAM

Results:

- Intel System (CPU only, 4 OpenMP threads):      0.203 ns/day
- Intel System (GPU accel., 4 OpenMP threads):    6.302 ns/day
- Apple System (CPU only, 8 OpenMP threads):     23.063 ns/day 
- Apple System (GPU accel., 8 OpenMP threads):   18.926 ns/day

Compare that to the Nvidia Titan Xp GPU, which according to the tutorial yields 295 ns/day
 (be aware that the tutorial is a bit older; GPU algorithm changed since than, also CPU version plays a role, so results are not 100 percent comparable). Some data to put this into perspective:

- Intel Iris Graphics 550:      0.77 TFLOPS (FP32)
- Apple M1 GPU:                 2.60 TFLOPS (FP32)
- Nvidia Titan Xp:             12.15 TFLOPS (FP32)

MD simulations are pretty much compute bound and not memory bound.
The M1 CPU performance is extremely impressive.
Note that the Macbook Air has a TDP of 10 W while the Titan Xp has a TDP of 250 W and it is only 12 times faster than the M1 CPU!!!!
The Intel and Apple integrated GPUs seem to not use its full potential. One reason might be power constraints on the die which is shared between CPU and GPU. So the GPU might be throttled. Also the GROMACS compute kernels might not be fully optimized for this hardware. During the simulation, the Intel GPU had a utilization between 50 to 65 percent, while matmul kernel execution yields 100 percent utilization. In the case of Apple (see also this thread) the deprecated OpenCL implementation (it is just a wrapper around the Metal Framework) could be another factor.

## Links


## Inspecting OpenCL kernels
On each timestep, several kernels are executed for force calculation:

### PME spline/spread
Direct submission (no waiting for event).
```sh
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
KERNEL_ISA
SCRATCH_SURFACE
LINEAR_STREAM
INTERNAL_HEAP
LINEAR_STREAM
TAG_BUFFER
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER
```

### k\_pruneonly
Direct submission.
```sh
CONSTANT_SURFACE
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
KERNEL_ISA
PROFILING_TAG_BUFFER
SCRATCH_SURFACE
LINEAR_STREAM
INTERNAL_HEAP
LINEAR_STREAM
TAG_BUFFER
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER
```

### k\_calc\_nb
Direct submission.
```sh
CONSTANT_SURFACE
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
KERNEL_ISA
PROFILING_TAG_BUFFER
SCRATCH_SURFACE
LINEAR_STREAM
INTERNAL_HEAP
LINEAR_STREAM
TAG_BUFFER
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER
```

### PME solve
Direct submission.
```sh
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
KERNEL_ISA
SCRATCH_SURFACE
LINEAR_STREAM
INTERNAL_HEAP
LINEAR_STREAM
TAG_BUFFER
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER
```

### PME gather
Direct submission.
```sh
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
BUFFER_HOST_MEMORY
KERNEL_ISA
SCRATCH_SURFACE
LINEAR_STREAM
INTERNAL_HEAP
LINEAR_STREAM
TAG_BUFFER
PREEMPTION
KERNEL_ISA_INTERNAL
COMMAND_BUFFER
```




