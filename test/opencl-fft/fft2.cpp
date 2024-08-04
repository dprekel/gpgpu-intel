// Kernel to perform the FFT on complex data
void fft1D(__global float* data,
           __global float* result,
           int n,
           int step) {
    int id = get_global_id(0);

    if (step < n) {
        int step2 = step * 2;

        // Perform butterfly operation
        float even = data[id];
        float odd = data[id + step];

        float angle = -M_PI * (id % step) / step;
        float twiddle = (float)(cos(angle), sin(angle));

        float temp = (float)(twiddle.x * odd.x - twiddle.y * odd.y,
                               twiddle.x * odd.y + twiddle.y * odd.x);

        result[id / step2 * step2 + (id % step)] = even + temp;
        result[id / step2 * step2 + (id % step) + step] = even - temp;
    }
}

// Wrapper kernel for multiple steps
__kernel void fft1D_recursive(__global float* data,
                              __global float* result,
                              int n) {
    for (int step = 1; step < n; step *= 2) {
        int numThreads = n / (2 * step);

        // Launch a 1D-FFT step
        fft1D(data, result, n, step);

        // Wait for all threads
        barrier(CLK_GLOBAL_MEM_FENCE);
        
        // Swap pointers for next step
        __global float* tmp = data;
        data = result;
        result = tmp;
    }
}

