#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <CL/cl.h>

const char* loadProgramSource(const char* filename, uint64_t* size);
uint64_t nanos();
void gemm();

void gemm() {

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
    printf("Device clock frequency: %u\n", device_clock);
 

    cl_context context = clCreateContext(NULL, num_devices, deviceStruct, 0, 0, &err);

    cl_command_queue queue = clCreateCommandQueue(context, *deviceStruct, 0, &err);


    const char* build_options = "-DTILE_SIZE_M=1 -DTILE_GROUP_M=1 -DTILE_SIZE_N=1 -DTILE_GROUP_N=1 -DTILE_SIZE_K=1";
    uint64_t sizeSource;
    const char* raw_text = loadProgramSource("gemm.cl", &sizeSource);
    printf("%s\n", raw_text);
    cl_program program = clCreateProgramWithSource(context, 1, &raw_text, 0, &err);
    printf("err: %d\n", err);

    err = clBuildProgram(program, num_devices, deviceStruct, build_options, 0, 0);
    printf("err: %d\n", err);

    const char* kernel_name = "GEMM";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
    printf("err: %d\n", err);

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
    
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_A, &err);
    printf("err: %d\n", err);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_B, &err);
    printf("err: %d\n", err);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, matrix_memory_size, matrix_C, &err);
    printf("err: %d\n", err);
    

    err = clSetKernelArg(kernel, 0, sizeof(int), (void*)&size);
    err = clSetKernelArg(kernel, 1, sizeof(int), (void*)&size);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*)&size);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferA);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufferB);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferC);
    printf("err: %d\n", err);

    // Maximum number of work items that make up a work group
    size_t max_work_group_size = 0;
    err = clGetDeviceInfo(*deviceStruct, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, 0);
    printf("CL_DEVICE_MAX_WORK_GROUP_SIZE: %lu\n", max_work_group_size);

    const size_t TS = 16;
    // number of work items per work group dimension
    const size_t local[2] = {TS, TS};
    // total number of work items in each dimension
    const size_t global[2] = {64, 64};


    for (int i = 0; i < 10; i++) {
        uint64_t start = nanos();
        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, 0, 0);
        printf("err: %d\n", err);
        err = clFinish(queue);
        uint64_t end = nanos();
        double time = (end - start)/1e6;
        printf("Runtime: %f ms\n", time);
        float* result_C = (float*)malloc(matrix_memory_size);
        err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, matrix_memory_size, result_C, 0, NULL, NULL);
        printf("result_C[0] = %f\n", result_C[0]);
        printf("result_C[size*100] = %f\n", result_C[size *100]);
        printf("result_C[matrix_size+1] = %f\n", result_C[matrix_size + 1]);
        free(result_C);
    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

}


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




int main() {
    gemm();
    return 0;
}




















