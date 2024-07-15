#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <CL/cl.h>

const char* loadProgramSource(const char* filename, uint64_t* size);
void directAccess();
uint64_t nanos();
float rand_uniform_01();


float rand_uniform_01() {
    return static_cast<float>(std::rand())/RAND_MAX;
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
    const char* raw_text = loadProgramSource("montecarlo.cl", &sizeSource);
    cl_program program = clCreateProgramWithSource(context, 1, &raw_text, 0, &err);
    printf("err: %d\n", err);

    size_t nsamples = 262144u;
    size_t noptions = 262144u;
    size_t array_memory_size = noptions * sizeof(float);
    std::string build_options = std::string("-D__DO_FLOAT__ ") +
                                std::string("-cl-denorms-are-zero ") +
                                std::string("-cl-fast-relaxed-math ") +
                                std::string("-cl-single-precision-constant ") +
                                std::string("-DNSAMP=") +
                                std::to_string(nsamples);
    err = clBuildProgram(program, num_devices, deviceStruct, build_options.c_str(), 0, 0);
    printf("err: %d\n", err);
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

    const char* kernel_name = "MonteCarloEuroOptCLKernelScalarBoxMuller";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
    printf("err: %d\n", err);

    float* s0Host = (float*)alignedMalloc(array_memory_size);       // Stock price
    float* xHost = (float*)alignedMalloc(array_memory_size);        // Strike price
    float* tHost = (float*)alignedMalloc(array_memory_size);        // Time
    float* vcallHost = (float*)alignedMalloc(array_memory_size);    // Zero result buffer
    float* vputHost = (float*)alignedMalloc(array_memory_size);     // Zero result buffer
    float* vput_refHost = (float*)alignedMalloc(array_memory_size); // Zero result buffer
    float* vcall_refHost = (float*)alignedMalloc(array_memory_size); // Zero result buffer

    std::generate_n(s0Host, noptions, [](){ return rand_uniform_01();});
    float S0L = 10.0f;
    float S0H = 50.0f;
    for (size_t i = 0; i < noptions; i++) {
        s0Host[i] = s0Host[i] * (S0H - S0L) + S0L;
    }
    std::generate_n(xHost, noptions, [](){ return rand_uniform_01();});
    float XL = 10.0f;
    float XH = 50.0f;
    for (size_t i = 0; i < noptions; i++) {
        xHost[i] = xHost[i] * (XH - XL) + XL;
    }
    std::generate_n(tHost, noptions, [](){ return rand_uniform_01();});
    float TL = 10.0f;
    float TH = 50.0f;
    for (size_t i = 0; i < noptions; i++) {
        tHost[i] = tHost[i] * (TH - TL) + TL;
    }
    for (size_t i = 0; i < noptions; i++) {
        vcallHost[i] = vputHost[i] = vcall_refHost[i] = vput_refHost[i] = 0.0f;
    }
    
    cl_mem s0 = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, array_memory_size, s0Host, &err);
    //printf("err: %d\n", err);
    cl_mem x = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, array_memory_size, xHost, &err);
    //printf("err: %d\n", err);
    cl_mem t = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, array_memory_size, tHost, &err);
    //printf("err: %d\n", err);
    cl_mem vcall = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, array_memory_size, vcallHost, &err);
    //printf("err: %d\n", err);
    cl_mem vput = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, array_memory_size, vputHost, &err);
    
    float risk_free = 0.05f;
    float sigma = 0.2f;
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&vcall);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&vput);
    err = clSetKernelArg(kernel, 2, sizeof(float), (void*)&risk_free);
    err = clSetKernelArg(kernel, 3, sizeof(float), (void*)&sigma);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&s0);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&x);
    err = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&t);
    printf("err: %d\n", err);

    size_t* local_size = nullptr;
    size_t global_size[1] = {noptions};

    for (int i = 0; i < 1; i++) {
        uint64_t start = nanos();
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_size, local_size, 0, 0, 0);
        printf("err: %d\n", err);
        err = clFinish(queue);
        uint64_t end = nanos();
        double time = (end - start)/1e6;
        printf("Runtime: %f ms\n", time);
        // we need to use clEnqueueReadBuffer because the memory was remapped by clCreateBuffer
        float* vputHostRead = (float*)malloc(array_memory_size);
        //err = clEnqueueReadBuffer(queue, vput, CL_TRUE, 0, array_memory_size, vputHostRead, 0, NULL, NULL);
        //sleep(15);
        /*
        printf("matrix_C[0] = %f\n", matrix_C[0]);
        printf("matrix_C[size*100] = %f\n", matrix_C[size *100]);
        printf("matrix_C[matrix_size-1] = %f\n", matrix_C[matrix_size - 1]);
        printf("matrix_C[matrix_size] = %f\n", matrix_C[matrix_size]);
        */
        free(vputHostRead);
    }

    clReleaseMemObject(s0);
    clReleaseMemObject(x);
    clReleaseMemObject(t);
    clReleaseMemObject(vput);
    clReleaseMemObject(vcall);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
