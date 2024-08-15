#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>

#include "pme.h"

#define CHECK_ERR(err, string)                      \
do {                                                \
    printf("[DEBUG] %s: %d\n", string, err);        \
    if (err)                                        \
        return err;                                 \
} while (0)

#define CHECK_ERROR(err)                            \
do {                                                \
    if (err) return err;                            \
} while (0)


int readBuffersFromFile(const char* filename, void* buffer) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("[DEBUG] Error opening file!\n");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);
    char* buf = static_cast<char*>(buffer);
    fread(buf, 1, size*sizeof(char), file);
    buf[size] = '\0';
    fclose(file);
    return SUCCESS;
}

int calculateNonbondedInteractions(pContext* context, pKernel* kernel) {
    int err = 0;

    cl_nbparam_params params;
    params.elecType                       = ElecType::EwaldAna;
    params.vdwType                        = VdwType::CutCombGeom;
    params.epsfac                         = 138.935455;
    params.c_rf                           = 1.0;
    params.two_k_rf                       = 0.0;
    params.ewald_beta                     = 3.12341309;
    params.sh_ewald                       = 1.00000125e-05;
    params.sh_lj_ewald                    = 0.0;
    params.ewaldcoeff_lj                  = 0.0;
    params.rcoulomb_sq                    = 1.0;
    params.rvdw_sq                        = 1.0;
    params.rvdw_switch                    = 0.0;
    params.rlistOuter_sq                  = 1.32249999;
    params.rlistInner_sq                  = 1.00200105;
    params.dispersion_shift.c2            = 0.0;
    params.dispersion_shift.c3            = 0.0;
    params.dispersion_shift.cpot          = -1.0;
    params.repulsion_shift.c2             = 0.0;
    params.repulsion_shift.c3             = 0.0;
    params.repulsion_shift.cpot           = -1.0;
    params.vdw_switch.c3                  = 0.0;
    params.vdw_switch.c4                  = 0.0;
    params.vdw_switch.c5                  = 0.0;
    params.coulomb_tab_scale              = 7.71402799e+31;

    int bCalcFshift                       = 1;

    size_t atomCoordinatesAndChargesSize  =  804896;  // IN
    size_t atomicForcesArraySize          =  603672;  // OUT
    size_t lennardJonesEnergySize         =       4;  // OUT
    size_t electrostaticsEnergySize       =       4;  // OUT
    size_t shiftForcesSize                =     540;  // OUT
    size_t LJSqrtsSize                    =  402448;  //
    size_t shiftVectorSize                =     540;  // IN
    size_t pairListIClustersSize          =   55344;  // IN
    size_t pairListJClustersSize          = 2549600;  // OUT/IN
    size_t atomInteractionBitsSize        =  409216;  // IN

    pBuffer* atomCoordinatesAndCharges = CreateBuffer(context, atomCoordinatesAndChargesSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* atomicForcesArray = CreateBuffer(context, atomicForcesArraySize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* lennardJonesEnergy = CreateBuffer(context, lennardJonesEnergySize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* electrostaticsEnergy = CreateBuffer(context, electrostaticsEnergySize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* shiftForces = CreateBuffer(context, shiftForcesSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* LJSqrts = CreateBuffer(context, LJSqrtsSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* shiftVector = CreateBuffer(context, shiftVectorSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* pairListIClusters = CreateBuffer(context, pairListIClustersSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* pairListJClusters = CreateBuffer(context, pairListJClustersSize, &err);
    //CHECK_ERR(err, "CreateBuffer");
    pBuffer* atomInteractionBits = CreateBuffer(context, atomInteractionBitsSize, &err);
    //CHECK_ERR(err, "CreateBuffer");

    err = readBuffersFromFile("buffer01.bin", atomCoordinatesAndCharges->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer02.bin", atomicForcesArray->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer03.bin", lennardJonesEnergy->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer04.bin", electrostaticsEnergy->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer05.bin", shiftForces->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer06.bin", LJSqrts->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer07.bin", shiftVector->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer11.bin", pairListIClusters->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer12.bin", pairListJClusters->mem);
    CHECK_ERROR(err);
    err = readBuffersFromFile("buffer13.bin", atomInteractionBits->mem);
    CHECK_ERROR(err);

    err = SetKernelArg(kernel, 0, sizeof(cl_nbparam_params), static_cast<void*>(&params));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 1, sizeof(pBuffer), static_cast<void*>(atomCoordinatesAndCharges));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 2, sizeof(pBuffer), static_cast<void*>(atomicForcesArray));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 3, sizeof(pBuffer), static_cast<void*>(lennardJonesEnergy));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 4, sizeof(pBuffer), static_cast<void*>(electrostaticsEnergy));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 5, sizeof(pBuffer), static_cast<void*>(shiftForces));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 6, sizeof(pBuffer), static_cast<void*>(LJSqrts));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 7, sizeof(pBuffer), static_cast<void*>(shiftVector));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 8, 0, nullptr);
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 9, 0, nullptr);
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 10, 0, nullptr);
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 11, sizeof(pBuffer), static_cast<void*>(pairListIClusters));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 12, sizeof(pBuffer), static_cast<void*>(pairListJClusters));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 13, sizeof(pBuffer), static_cast<void*>(atomInteractionBits));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 14, sizeof(int), static_cast<void*>(&bCalcFshift));
    //printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 15, 1000, nullptr);
    //printf("[DEBUG] SetKernelArg: %d\n", err);

    const size_t local[3] = {4, 4, 1};
    const size_t global[3] = {8268, 4, 1};
    err = ExecuteKernel(context, kernel, 3, global, local);
    if (err) {
        printf("[DEBUG] Batchbuffer failed with %d\n", err);
    }

    err = ReleaseBuffer(atomCoordinatesAndCharges);
    err = ReleaseBuffer(atomicForcesArray);
    err = ReleaseBuffer(lennardJonesEnergy);
    err = ReleaseBuffer(electrostaticsEnergy);
    err = ReleaseBuffer(shiftForces);
    err = ReleaseBuffer(LJSqrts);
    err = ReleaseBuffer(shiftVector);
    err = ReleaseBuffer(pairListIClusters);
    err = ReleaseBuffer(pairListJClusters);
    err = ReleaseBuffer(atomInteractionBits);

    return err;
}


int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    //printf("[DEBUG] CreateDevices: %d\n", err);

    std::string build_options = std::string("-DWARP_SIZE_TEST=32")
                              + " -D_INTEL_SOURCE_"
                              + " -DGMX_OCL_FASTGEN_ADD_TWINCUT"
                              + " -DEL_EWALD_ANA"
                              + " -DEELNAME=_ElecEw"
                              + " -DLJ_COMB_GEOM"
                              + " -DVDWNAME=_VdwLJCombGeom"
                              + " -Dc_nbnxnGpuClusterSize=4"
                              + " -DNBNXM_MIN_DISTANCE_SQUARED_VALUE_FLOAT=3.82e-07"
                              + " -Dc_nbnxnGpuNumClusterPerSupercluster=8"
                              + " -Dc_nbnxnGpuJgroupSize=4"
                              + " -Dc_centralShiftIndex=22"
                              + " -DIATYPE_SHMEM";

    pContext* context = CreateContext(devices, 0u, &err);
    //printf("[DEBUG] CreateContext: %d\n", err);
    pKernel* kernel = BuildKernel(context, "nbnxm_ocl_kernels.cl", build_options.c_str(), true, &err);
    //printf("[DEBUG] BuildKernel: %d\n", err);

    for (int i = 0; i < 60; i++) {
        calculateNonbondedInteractions(context, kernel);
    }

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseDevice(devices, 0u);
    return 0;
}















