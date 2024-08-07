__attribute__((reqd_work_group_size(TILE_GROUP_M, TILE_GROUP_N, 1)))
kernel void matmul(global const float* restrict A,
                    int lda,
                    global const float* restrict B,
                    int ldb,
                    global float* restrict C,
                    int ldc,
                    int k)
{
    int Aind = get_group_id(0)*TILE_GROUP_M*TILE_SIZE_M + get_local_id(0);
    int Bind = get_group_id(1)*TILE_GROUP_N*TILE_SIZE_N + get_local_id(1);
    int Cind = Aind + Bind*ldc;

    float c[TILE_SIZE_M*TILE_SIZE_N] = {(float)0};

    for (int l = 0; l < k; l++) {
        for (int i = 0; i < TILE_SIZE_M; i++) {
            for (int j = 0; j < TILE_SIZE_N; j++) {
                c[i*TILE_SIZE_N + j] += A[Aind + i*TILE_GROUP_M] * B[Bind + j*TILE_GROUP_N];
            }
        }
        Aind += lda;
        Bind += ldb;
    }

    for (int i = 0; i < TILE_SIZE_M; i++) {
        for (int j = 0; j < TILE_SIZE_N; j++) {
            int Ccur = Cind + i*TILE_GROUP_M + j*TILE_GROUP_N*ldc;
            C[Ccur] = c[i*TILE_SIZE_N + j];
        }
    }
}



__attribute__((reqd_work_group_size(TILE_GROUP_M, TILE_GROUP_N, 1)))
kernel void gemm_nn (
    global const float* restrict A,
    int lda,    // column stride in elements for matrix A
    global const float* restrict B,
    int ldb,    // column stride in elements for matrix B
    global float* restrict C,
    int ldc,    // column stride in elements for matrix C
    int k,
    float alpha,
    float beta
)
{

    int Aind = get_group_id(0)*TILE_GROUP_M*TILE_SIZE_M + get_local_id(0);
    int Bind = get_group_id(1)*TILE_GROUP_N*TILE_SIZE_N + get_local_id(1);
    int Cind = Aind + Bind*ldc;

    Bind *= ldb;    // matrix B is in column-major form

    float c[TILE_SIZE_M*TILE_SIZE_N] = {(float)0};

    // Main accumulation loop
    for(int l_block = 0; l_block < k; l_block += TILE_SIZE_K)
    {
        for(int i = 0; i < TILE_SIZE_M; ++i)
            for(int j = 0; j < TILE_SIZE_N; ++j)
                for(int l = 0; l < TILE_SIZE_K; ++l)
                    c[i*TILE_SIZE_N + j] +=
                        A[Aind + l*lda + i*TILE_GROUP_M] *
                        B[Bind + l + j*ldb*TILE_GROUP_N];
        Aind += lda*TILE_SIZE_K;
        Bind += TILE_SIZE_K;
    }

    // Store accumulated results from c to C with alpha and beta multiplication
    for(int i = 0; i < TILE_SIZE_M; ++i)
        for(int j = 0; j < TILE_SIZE_N; ++j)
        {
            int Ccur = Cind + i*TILE_GROUP_M + j*TILE_GROUP_N*ldc;
            C[Ccur] = alpha*c[i*TILE_SIZE_N + j] + beta*C[Ccur];
        }
}
