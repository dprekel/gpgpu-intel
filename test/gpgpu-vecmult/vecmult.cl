#include "functions.cl"

__kernel void square_elements(__global const float* input,
                              __global float* output) {
    int gid = get_global_id(0);
    output[gid] = square(input[gid]);
}

