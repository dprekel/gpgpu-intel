// First naive implementation
__kernel void GEMM(const int M,
                   const int N,
                   const int K, 
                   const __global float* A, 
                   const __global float* B, 
                   __global float* C) {

    // Thread identifiers
    const int globalRow = get_global_id(0);
    const int globalCol = get_global_id(1);

    // Compute a single element (loop over K)
    float acc = 0.0f;
    for (int k = 0; k < K; k++) {
        acc = acc + A[k*M + globalRow] * B[globalCol*K + k];
    }

    // Store the result
    C[globalCol*M + globalRow] = acc;
}
