#include <gpgpu.h>
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

    size_t nsamples = 262144u;
    std::string build_options = std::string("-D__DO_FLOAT__ ") +
                                std::string("-cl-denorms-are-zero ") +
                                std::string("-cl-fast-relaxed-math ") +
                                std::string("-cl-single-precision-constant ") +
                                std::string("-DNSAMP=") +
                                std::to_string(nsamples);
    pContext* context = CreateContext(devices, 0u, &err);
    pKernel* kernel = BuildKernel(context, "montecarlo.cl", build_options.c_str(), true, &err);

    size_t noptions = 65536u;
    size_t array_memory_size = noptions * sizeof(float);
    
    pBuffer* s0Host = CreateBuffer(context, array_memory_size, &err);
    pBuffer* xHost = CreateBuffer(context, array_memory_size, &err);
    pBuffer* tHost = CreateBuffer(context, array_memory_size, &err);
    pBuffer* vcallHost = CreateBuffer(context, array_memory_size, &err);
    pBuffer* vputHost = CreateBuffer(context, array_memory_size, &err);
    for (size_t i = 0; i < noptions; i++) {
        float* s0HostMem = static_cast<float*>(s0Host->mem);
        s0HostMem[i] = 1.0f;
        float* xHostMem = static_cast<float*>(xHost->mem);
        xHostMem[i] = 1.0f;
        float* tHostMem = static_cast<float*>(tHost->mem);
        tHostMem[i] = 1.0f;
        float* vcallHostMem = static_cast<float*>(vcallHost->mem);
        float* vputHostMem = static_cast<float*>(vputHost->mem);
        vcallHostMem[i] = vputHostMem[i] = 0.0f;
    }

    float risk_free = 0.05f;
    float sigma = 0.2f;
    err = SetKernelArg(kernel, 0, sizeof(vcallHost), static_cast<void*>(&vcallHost));
    err = SetKernelArg(kernel, 1, sizeof(vputHost),  static_cast<void*>(&vputHost));
    err = SetKernelArg(kernel, 2, sizeof(float),     static_cast<void*>(&risk_free));
    err = SetKernelArg(kernel, 3, sizeof(float),     static_cast<void*>(&sigma));
    err = SetKernelArg(kernel, 4, sizeof(s0Host),    static_cast<void*>(&s0Host));
    err = SetKernelArg(kernel, 5, sizeof(xHost),     static_cast<void*>(&xHost));
    err = SetKernelArg(kernel, 6, sizeof(tHost),     static_cast<void*>(&tHost));

    const size_t* local = nullptr;
    const size_t global[1] = {noptions};

    for (int i = 0; i < 10; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 1, global, local);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        /*
        printf("  matCMem[0] = %f\n", matCMem[0]);
        printf("  matCMem[size*100] = %f\n", matCMem[size *100]);
        printf("  matCMem[7500000] = %f\n", matCMem[7500000]);
        printf("  matCMem[matrix_size-10] = %f\n", matCMem[matrix_size - 10]);
        printf("  matCMem[matrix_size] = %f\n", matCMem[matrix_size]);
        */
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
    err = ReleaseDevice(devices, 0u);
    return 0;
}
