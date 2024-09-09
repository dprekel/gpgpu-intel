#include <gpgpu_api.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>


int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    printf("[DEBUG] CreateDevices: %d\n", err);

    size_t nsamples = 262144u;
    std::string build_options = std::string("-D__DO_FLOAT__ ") +
                                std::string("-cl-denorms-are-zero ") +
                                std::string("-cl-fast-relaxed-math ") +
                                std::string("-cl-single-precision-constant ") +
                                std::string("-DNSAMP=") +
                                std::to_string(nsamples);
    pContext* context = CreateContext(devices, 0u, &err);
    printf("[DEBUG] CreateContext: %d\n", err);
    pKernel* kernel = BuildKernel(context, "montecarlo.cl", build_options.c_str(), true, &err);
    printf("[DEBUG] BuildKernel: %d\n", err);

    size_t noptions = 65536u;
    size_t array_memory_size = noptions * sizeof(float);
    
    pBuffer* s0Host = CreateBuffer(context, array_memory_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    pBuffer* xHost = CreateBuffer(context, array_memory_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    pBuffer* tHost = CreateBuffer(context, array_memory_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    pBuffer* vcallHost = CreateBuffer(context, array_memory_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    pBuffer* vputHost = CreateBuffer(context, array_memory_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    float* s0HostMem = static_cast<float*>(s0Host->mem);
    float* xHostMem = static_cast<float*>(xHost->mem);
    float* tHostMem = static_cast<float*>(tHost->mem);
    float* vcallHostMem = static_cast<float*>(vcallHost->mem);
    float* vputHostMem = static_cast<float*>(vputHost->mem);
    for (size_t i = 0; i < noptions; i++) {
        s0HostMem[i] = 1.0f;
        xHostMem[i] = 1.0f;
        tHostMem[i] = 1.0f;
        vcallHostMem[i] = vputHostMem[i] = 0.0f;
    }

    float risk_free = 0.05f;
    float sigma = 0.2f;
    err = SetKernelArg(kernel, 0, sizeof(vcallHost), static_cast<void*>(vcallHost));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 1, sizeof(vputHost),  static_cast<void*>(vputHost));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 2, sizeof(float),     static_cast<void*>(&risk_free));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 3, sizeof(float),     static_cast<void*>(&sigma));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 4, sizeof(s0Host),    static_cast<void*>(s0Host));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 5, sizeof(xHost),     static_cast<void*>(xHost));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 6, sizeof(tHost),     static_cast<void*>(tHost));
    printf("[DEBUG] SetKernelArg: %d\n", err);

    const size_t local[3] = {256, 1, 1};
    const size_t global[3] = {noptions, 1, 1};

    for (int i = 0; i < 1; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 1, global, local);
        printf("[DEBUG] ExecuteKernel: %d\n", err);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        printf("  vcallHostMem[0] = %f\n", vcallHostMem[0]);
        printf("  vcallHostMem[100] = %f\n", vcallHostMem[100]);
        printf("  vcallHostMem[noptions-1] = %f\n", vcallHostMem[noptions - 1]);
        printf("  vcallHostMem[noptions] = %f\n", vcallHostMem[noptions]);
        /*
        printf("[DEBUG] Checking validity ...  ");
        fflush(stdout);
        std::chrono::high_resolution_clock::time_point time1, time2;
        time1 = std::chrono::high_resolution_clock::now();
        bool valid = checkValidity(matAMem, matBMem, matCMem, size, size, true);
        time2 = std::chrono::high_resolution_clock::now();
        int64_t elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(time2 - time1).count();
        if (!valid) {
            printf("Failed! (Execution time: %.2f seconds)\n", elapsedTime/1e9);
        } else {
            printf("Passed! (Execution time: %.2f seconds)\n", elapsedTime/1e9);
        }
        */
        /*
        for (size_t i = 0; i < matrix_size; i++) {
            matCMem[i] = 0.0;
        }
        */
    }
    printf("\n");

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseBuffer(s0Host);
    err = ReleaseBuffer(xHost);
    err = ReleaseBuffer(tHost);
    err = ReleaseBuffer(vcallHost);
    err = ReleaseBuffer(vputHost);
    err = ReleaseDevices(devices);
    return 0;
}
