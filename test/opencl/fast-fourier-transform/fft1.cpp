
#define FFT_PI       3.14159265359f
#define FFT_SQRT_1_2 0.707106781187f

float mul(float a, float b) {
    return (float)(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float twiddle(float a, int k, float alpha) {
    float cs;
    float sn;
    sn = sincos((float)k * alpha, &cs);
    return mul(a, (float)(cs, sn));
}

#define DFT2(a,b) { float tmp = a - b; a += b; b = tmp; }

__kernel void fftRadix2Kernel(__global const float* x,
                              __global float* y,
                              int p) {
    int t = get_global_size(0);
    int i = get_global_id(0);
    int k = i & (p-1); 
    int j = ((i-k) << 1) + k;
    float alpha = -FFT_PI * (float)k / (float)p;

    // Read and twiddle input
    x += i;
    float u0 = x[0];
    float u1 = twiddle(x[t], 1, alpha);

    // In-place DFT-2
    //DFT2(u0, u1);
    {
        float tmp = u0 - u1;
        u0 += u1;
        u1 = tmp;
    }

    // Write output
    y += j;
    y[0] = u0;
    y[p] = u1;
}





