#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>

#define CHECK_ERR(err, string)                      \
do {                                                \
    printf("[DEBUG] %s: %d\n", string, err);        \
    if (err)                                        \
        return err;                                 \
} while (0)

int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    printf("[DEBUG] CreateDevices: %d\n", err);

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
    printf("[DEBUG] CreateContext: %d\n", err);
    pKernel* kernel = BuildKernel(context, "nbnxm_ocl_kernels.cl", build_options.c_str(), true, &err);
    printf("[DEBUG] BuildKernel: %d\n", err);

    size_t paramsSize                       =       0;
    size_t atomCoordinatesAndChargesSize    =  804896;
    size_t atomicForcesArraySize            =  603672;
    size_t lennardJonesEnergySize           =       4;
    size_t electrostaticsEnergySize         =       4;
    size_t shiftForcesSize                  =     540;
    size_t LJSqrtsSize                      =  402448;
    size_t shiftVectorSize                  =     540;
    size_t pairListIClustersSize            =   55344;
    size_t pairListJClustersSize            = 2549600;
    size_t atomInteractionBitsSize          =  409216;

    pBuffer* atomCoordinatesAndCharges = CreateBuffer(context, atomCoordinatesAndChargesSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* atomicForcesArray = CreateBuffer(context, atomicForcesArraySize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* lennardJonesEnergy = CreateBuffer(context, lennardJonesEnergySize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* electrostaticsEnergy = CreateBuffer(context, electrostaticsEnergySize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* shiftForces = CreateBuffer(context, shiftForcesSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* LJSqrts = CreateBuffer(context, LJSqrtsSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* shiftVector = CreateBuffer(context, shiftVectorSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* pairListIClusters = CreateBuffer(context, pairListIClustersSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* pairListJClusters = CreateBuffer(context, pairListJClustersSize, &err);
    CHECK_ERR(err, "CreateBuffer");
    pBuffer* atomInteractionBits = CreateBuffer(context, atomInteractionBitsSize, &err);
    CHECK_ERR(err, "CreateBuffer");

    //err = ReleaseKernel(kernel);
    //err = ReleaseContext(context);
    err = ReleaseDevice(devices, 0u);
    return 0;
}









