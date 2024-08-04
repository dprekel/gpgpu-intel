__kernel void vectorAdd(__global const float* a,
                        __global const float* b,
                        __global float* result) {
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int localSize = get_local_size(0);

    localBuffer[lid] = a[gid] + b[gid];
    __local float localBuf[1000];

    barrier(CLK_LOCAL_MEM_FENCE);

    result[gid] = localBuffer[lid];
}
