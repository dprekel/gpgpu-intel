#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <CL/cl.h>

#define TILE_SIZE_K     8
#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

const char* loadProgramSource(const char* filename, uint64_t* size);
void directAccess();
uint64_t nanos();


const char* loadProgramSource(const char* filename, uint64_t* _size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = (char*)malloc((size+1)*sizeof(char));
    fread((void*)source, 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    *_size = size+1;
    return (const char*)source;
}

void* alignedMalloc(size_t size) {
    size_t alignment = 4096;
    size_t sizeToAlloc = size + alignment;
    void* pOriginalMemory = malloc(sizeToAlloc);

    uintptr_t pAlignedMemory = reinterpret_cast<uintptr_t>(pOriginalMemory);
    if (pAlignedMemory) {
        pAlignedMemory += alignment;
        pAlignedMemory -= pAlignedMemory % alignment;
        reinterpret_cast<void**>(pAlignedMemory)[-1] = pOriginalMemory;
    }
    else {
        return nullptr;
    }
    return reinterpret_cast<void*>(pAlignedMemory);
}

uint64_t nanos() {
     struct timespec start;
     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
     return (uint64_t)start.tv_sec*1000000000 + (uint64_t)start.tv_nsec;
}


int main() {

    cl_uint num_platforms = 0;
    cl_int err = clGetPlatformIDs(0, 0, &num_platforms);
    printf("num_platforms: %d\n", num_platforms);
    cl_platform_id* platformStruct = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
    err = clGetPlatformIDs(num_platforms, platformStruct, 0);

    cl_uint num_devices = 0;
    err = clGetDeviceIDs(*platformStruct, CL_DEVICE_TYPE_GPU, 0, 0, &num_devices);
    printf("num_devices: %d\n", num_devices);
    cl_device_id* deviceStruct = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
    err = clGetDeviceIDs(*platformStruct, CL_DEVICE_TYPE_GPU, num_devices, deviceStruct, 0);

    size_t size_device_clock = 0;
    err = clGetDeviceInfo(*deviceStruct, CL_DEVICE_MAX_CLOCK_FREQUENCY, 0, 0, &size_device_clock);
    cl_uint device_clock = 0;
    err = clGetDeviceInfo(*deviceStruct, CL_DEVICE_MAX_CLOCK_FREQUENCY, size_device_clock, &device_clock, 0);
    printf("Device clock frequency: %u MHz\n", device_clock);
 

    cl_context context = clCreateContext(NULL, num_devices, deviceStruct, 0, 0, &err);

    cl_command_queue queue = clCreateCommandQueue(context, *deviceStruct, 0, &err);


    uint64_t sizeSource;
    const char* raw_text = loadProgramSource("matmul.cl", &sizeSource);
    cl_program program = clCreateProgramWithSource(context, 1, &raw_text, 0, &err);
    printf("clCreateProgramWithSource: %d\n", err);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N)
                              + " -DTILE_SIZE_K=8" + std::to_string(TILE_SIZE_K);
    err = clBuildProgram(program, num_devices, deviceStruct, build_options.c_str(), 0, 0);
    printf("clBuildProgram: %d\n", err);
    if (err == CL_BUILD_PROGRAM_FAILURE) {
        size_t log_size;
        clGetProgramBuildInfo(program, *deviceStruct, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, *deviceStruct, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }
    size_t* programBinarySizes = nullptr;
    char* programBinary = nullptr;
    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), programBinarySizes, nullptr);
    if (err) {
        return err;
    }
    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(char*), programBinary, nullptr);
    if (err) {
        return err;
    }

    // Create kernel 1
    const char* kernel1_name = "matmul";
    cl_kernel kernel1 = clCreateKernel(program, kernel1_name, &err);
    printf("clCreateKernel: %d\n", err);

    // Create kernel 2
    const char* kernel2_name = "gemm_nn";
    cl_kernel kernel2 = clCreateKernel(program, kernel2_name, &err);
    printf("clCreateKernel: %d\n", err);

    // Allocating memory for matrices
    size_t size = 3968;
    size_t matrix_memory_size = size*size*sizeof(float);
    float* matrix_A = (float*)alignedMalloc(matrix_memory_size);
    float* matrix_B = (float*)alignedMalloc(matrix_memory_size);
    float* matrix_C = (float*)alignedMalloc(matrix_memory_size);

    // Initialize matrices A and B with ones
    size_t matrix_size = size*size;
    for (size_t i = 0; i < matrix_size; i++) {
        matrix_A[i] = 1.0;
        matrix_B[i] = 1.0;
    }
    
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_A, &err);
    printf("clCreateBuffer: %d\n", err);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_B, &err);
    printf("clCreateBuffer: %d\n", err);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_C, &err);
    printf("clCreateBuffer: %d\n", err);
    

    // Arguments for Kernel 1:
    cl_int ldabc = static_cast<int>(size);
    err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void*)&bufferA);
    err = clSetKernelArg(kernel1, 1, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel1, 2, sizeof(cl_mem), (void*)&bufferB);
    err = clSetKernelArg(kernel1, 3, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel1, 4, sizeof(cl_mem), (void*)&bufferC);
    err = clSetKernelArg(kernel1, 5, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel1, 6, sizeof(cl_int), (void*)&ldabc);
    printf("sizeof cl_mem: %lu\n", sizeof(cl_mem));
    printf("clSetKernelArg: %d\n", err);

    // Arguments for Kernel 2:
    float alpha = 1.0f;
    float beta = 0.0f;
    err = clSetKernelArg(kernel2, 0, sizeof(cl_mem), (void*)&bufferA);
    err = clSetKernelArg(kernel2, 1, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel2, 2, sizeof(cl_mem), (void*)&bufferB);
    err = clSetKernelArg(kernel2, 3, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel2, 4, sizeof(cl_mem), (void*)&bufferC);
    err = clSetKernelArg(kernel2, 5, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel2, 6, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel2, 7, sizeof(cl_int), (void*)&alpha);
    err = clSetKernelArg(kernel2, 8, sizeof(cl_int), (void*)&beta);
    printf("clSetKernelArg: %d\n", err);

    // number of work items per work group dimension
    const size_t local[2] = {TILE_GROUP_M, TILE_GROUP_N};
    // total number of work items in each dimension
    const size_t global[2] = {size/TILE_SIZE_M, size/TILE_SIZE_N};

    for (int i = 0; i < 10; i++) {

        // Execute Kernel 1:
        uint64_t start = nanos();
        printf("Start clEnqueueNDRangeKernel\n");
        err = clEnqueueNDRangeKernel(queue, kernel1, 2, NULL, global, local, 0, 0, 0);
        printf("clEnqueueNDRangeKernel: %d\n", err);
        err = clFinish(queue);
        uint64_t end = nanos();
        double time = (end - start)/1e6;
        printf("Runtime: %f ms\n", time);
        printf("matrix_C[0] = %f\n", matrix_C[0]);
        printf("matrix_C[size*100] = %f\n", matrix_C[size *100]);
        printf("matrix_C[matrix_size-1] = %f\n", matrix_C[matrix_size - 1]);
        printf("matrix_C[matrix_size] = %f\n", matrix_C[matrix_size]);

        // Execute Kernel 2:
        start = nanos();
        err = clEnqueueNDRangeKernel(queue, kernel2, 2, NULL, global, local, 0, 0, 0);
        printf("clEnqueueNDRangeKernel: %d\n", err);
        err = clFinish(queue);
        end = nanos();
        time = (end - start)/1e6;
        printf("Runtime: %f ms\n", time);
        printf("matrix_C[0] = %f\n", matrix_C[0]);
        printf("matrix_C[size*100] = %f\n", matrix_C[size *100]);
        printf("matrix_C[matrix_size-1] = %f\n", matrix_C[matrix_size - 1]);
        printf("matrix_C[matrix_size] = %f\n", matrix_C[matrix_size]);
    }

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel1);
    clReleaseKernel(kernel2);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}









