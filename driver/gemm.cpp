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

    uint64_t sizeSource;
    const char* raw_text = loadProgramSource("program.cl", &sizeSource);
    cl_program program = clCreateProgramWithSource(context, 1, &raw_text, 0, &err);
    printf("err: %d\n", err);

    err = clBuildProgram(program, num_devices, deviceStruct, 0, 0, 0);
    printf("err: %d\n", err);

    const char* kernel_name = "GEMM";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
    printf("err: %d\n", err);

    float A[900];
    float B[900];
    float C[900];
    int bufferSize = (int)sizeof(A)/(int)sizeof(float);
    for (int i = 0; i < bufferSize; i++) {
        A[i] = 2;
        B[i] = 2;
    }
    
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(A), &A, &err);
    printf("err: %d\n", err);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(B), &B, &err);
    printf("err: %d\n", err);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(C), &C, &err);
    printf("err: %d\n", err);
    
    size_t M = 30;
    size_t N = 30;
    size_t K = 30;
    err = clSetKernelArg(kernel, 0, sizeof(int), (void*)&M);
    err = clSetKernelArg(kernel, 1, sizeof(int), (void*)&M);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*)&M);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferA);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufferB);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferC);
    printf("err: %d\n", err);

    const size_t TS = 5;
    const size_t local[2] = {TS, TS};
    const size_t global[2] = {M, N};

    uint64_t start = nanos();

    cl_event event;
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    printf("err: %d\n", err);

    err = clWaitForEvents(1, &event);
    uint64_t end = nanos();
    double time = (end - start)/1e6;
    printf("Runtime: %f ms\n", time);

    for (int i = 0; i < 16; i++) {
        printf("Matrix C[%d] = %f\n", i, C[i]);
    }
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




















