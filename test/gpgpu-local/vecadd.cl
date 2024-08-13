__kernel void vectorAdd(__global const float* a,
                        __global const float* b,
                        __global float* result,
                        __local float* localMem1,
                        __local float* localMem2) {
    int global_id = get_global_id(0);
    int local_id = get_local_id(0);

    localMem1[local_id] = a[global_id];
    localMem2[local_id] = b[global_id];

    barrier(CLK_LOCAL_MEM_FENCE);

    float sum = localMem1[local_id] + localMem2[local_id];

    result[global_id] = sum;
}
