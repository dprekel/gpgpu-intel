#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <CL/cl.h>
#include "../driver/driver.h"

#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

const char* loadProgramSource(const char* filename, uint64_t* size);
void continueWithOpenCL(cl_context context, cl_command_queue queue, cl_kernel kernel);
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



uint64_t nanos() {
     struct timespec start;
     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
     return (uint64_t)start.tv_sec*1000000000 + (uint64_t)start.tv_nsec;
}


void continueWithOpenCL(cl_context context, cl_command_queue queue, cl_kernel kernel) {
    cl_int err = 0;

    // Allocating memory for matrices
    size_t size = 3968;
    size_t matrix_memory_size = size*size*sizeof(float);
    // for such big matrices, malloc should use mmap for allocating memory, so it will be page-aligned. But there is a problem: strace shows me that the driver is remapping the memory, I need a way to avoid this
    float* matrix_A = (float*)malloc(matrix_memory_size);
    float* matrix_B = (float*)malloc(matrix_memory_size);
    float* matrix_C = (float*)malloc(matrix_memory_size);

    // Initialize matrices A and B with ones
    size_t matrix_size = size*size;
    for (size_t i = 0; i < matrix_size; i++) {
        matrix_A[i] = 1.0;
        matrix_B[i] = 1.0;
    }
    
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_A, &err);
    printf("err: %d\n", err);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_B, &err);
    printf("err: %d\n", err);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_C, &err);
    printf("err: %d\n", err);
    

    cl_int ldabc = static_cast<int>(size);
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&bufferA);
    err = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferB);
    err = clSetKernelArg(kernel, 3, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufferC);
    err = clSetKernelArg(kernel, 5, sizeof(cl_int), (void*)&ldabc);
    err = clSetKernelArg(kernel, 6, sizeof(cl_int), (void*)&ldabc);
    printf("err: %d\n", err);

    // number of work items per work group dimension
    const size_t local[2] = {TILE_GROUP_M, TILE_GROUP_N};
    // total number of work items in each dimension
    const size_t global[2] = {size/TILE_SIZE_M, size/TILE_SIZE_N};


    for (int i = 0; i < 3; i++) {
        uint64_t start = nanos();
        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, 0, 0);
        printf("err: %d\n", err);
        err = clFinish(queue);
        uint64_t end = nanos();
        double time = (end - start)/1e6;
        printf("Runtime: %f ms\n", time);
        // we need to use clEnqueueReadBuffer because the memory was remapped by clCreateBuffer
        float* result_C = (float*)malloc(matrix_memory_size);
        err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, matrix_memory_size, result_C, 0, NULL, NULL);
        printf("result_C[0] = %f\n", result_C[0]);
        printf("result_C[size*100] = %f\n", result_C[size *100]);
        printf("result_C[matrix_size-1] = %f\n", result_C[matrix_size - 1]);
        printf("result_C[matrix_size] = %f\n", result_C[matrix_size]);
        free(result_C);
    }

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
}



void directAccess() {
    // Allocating memory for matrices
    size_t size = 3968;
    size_t matrix_memory_size = size*size*sizeof(float);

    // call the driver for aligned memory allocation
    void* matrixA = allocateAndPinBuffer(matrix_memory_size);
    return;
}



int main(int argc, char** argv) {

    if (argc > 2) {
        printf("This program only takes one argument! Aborting...\n");
        return -1;
    }
    // command line arguments should be checked here

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
    printf("err: %d\n", err);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    err = clBuildProgram(program, num_devices, deviceStruct, build_options.c_str(), 0, 0);
    printf("err: %d\n", err);
    if (err == CL_BUILD_PROGRAM_FAILURE) {
        size_t log_size;
        clGetProgramBuildInfo(program, *deviceStruct, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, *deviceStruct, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }

    const char* kernel_name = "matmul";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
    printf("err: %d\n", err);


    if (0 == strcmp(argv[1], "no")) {
        continueWithOpenCL(context, queue, kernel);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
    }
    else if (0 == strcmp(argv[1], "yes")) {
        directAccess(); 
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
    }
    else {
        printf("Invalid command line argument! Aborting...\n");
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return -1;
    }
    return 0;
}
























