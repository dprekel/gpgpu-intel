#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>

#include "kernel.h"
#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"
#include "device.h"

#define COMPILER_LOAD_FAILED -1


Kernel::Kernel(Context* context, const char* filename, const char* options) 
         : context(context),
           filename(filename),
           options(options),
           igcName("libigc.so.1"),
           fclName("libigdfcl.so.1"), 
           srcType(2305843009183132750),            // oclC
           intermediateType(2305843009202725362),   // spirV
           outType(18425635491780865102) {          // oclGenBin
    this->optionsSize = strlen(options);
}

Kernel::~Kernel() {
    printf("Kernel destructor called!\n");
}

KernelFromPatchtokens* Kernel::getKernelData() {
    return &kernelData;
}

int Kernel::loadProgramSource() {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = new char[size+1];
    fread(static_cast<void*>(source), 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    this->srcCode = static_cast<const char*>(source);
    this->srcSize = strlen(source+1);
    //printf("%s\n", srcCode);
    return 0;
}

ICIF* Kernel::CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion) {
    uint64_t chosenVersion;
    uint64_t minVerSupported = 0;
    uint64_t maxVerSupported = 0;

    bool isSupported = cifMain->GetSupportedVersions(interfaceID, minVerSupported, maxVerSupported);
    //printf("maxVerSupported: %lu\n", maxVerSupported);
    //printf("minVerSupported: %lu\n", minVerSupported);
    if (isSupported == false) {
        return nullptr;
    }
    if ((interfaceVersion < minVerSupported) || (interfaceVersion > maxVerSupported)) {
        return nullptr;
    }
    printf("Versions are ok\n");
    chosenVersion = std::min(maxVerSupported, interfaceVersion);
    
    ICIF* deviceCtx = cifMain->CreateInterfaceImpl(interfaceID, chosenVersion);
    return deviceCtx;
}

IgcBuffer* Kernel::CreateIgcBuffer(CIFMain* cifMain, const char* data, size_t size) {
    if (cifMain == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = 1;
    auto buff = CreateInterface(cifMain, interfaceID, interfaceVersion);
    IgcBuffer* buffer = static_cast<IgcBuffer*>(buff);
    if (buffer == nullptr) {
        return nullptr;
    }
    if ((data != nullptr) && (size != 0)) {
        buffer->SetUnderlyingStorage(static_cast<const void*>(data), size);
    }
    return buffer;
}

int Kernel::loadCompiler(const char* libName, CIFMain** cifMain) {
    void* handle;
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    handle = dlopen(libName, dlopenFlag);
    if (!handle) {
        printf("Loading compiler library not successful\n");
        return COMPILER_LOAD_FAILED;
    }
    CIFMain* (*CreateCIFMainFunc)();
    void* addr = dlsym(handle, "CIFCreateMain");
    CreateCIFMainFunc = reinterpret_cast<CIFMain*(*)()>(addr);
    if (CreateCIFMainFunc == nullptr) {
        printf("Couldn't create main entry point\n");
        return COMPILER_LOAD_FAILED;
    }
    *cifMain = CreateCIFMainFunc();
    if (*cifMain == nullptr) {
        printf("CIFCreateMain failed\n");
        return COMPILER_LOAD_FAILED;
    }
    return 0;
}

FclOclTranslationCtx* Kernel::createFclTranslationCtx() {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 5;
    ICIF* DeviceCtx = CreateInterface(fclMain, interfaceID, interfaceVersion);
    FclOclDeviceCtx* newDeviceCtx = static_cast<FclOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        printf("No Device Context!\n");
        return nullptr;
    }
    uint32_t openCLVersion = 30;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        uint64_t platformVersion = 1;
        PlatformInfo* igcPlatform = newDeviceCtx->GetPlatformHandleImpl(platformVersion);
        if (nullptr == igcPlatform) {
            return nullptr;
        }
        TransferPlatformInfo(igcPlatform, descriptor->pHwInfo->platform);
    }
    uint64_t translationCtxVersion = 1;
    auto fclTranslationCtx = newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, srcType, intermediateType);
    return fclTranslationCtx;
}

void Kernel::TransferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform) {
    igcPlatform->SetProductFamily(platform->eProductFamily);
    igcPlatform->SetPCHProductFamily(platform->ePCHProductFamily);
    igcPlatform->SetDisplayCoreFamily(platform->eDisplayCoreFamily);
    igcPlatform->SetRenderCoreFamily(platform->eRenderCoreFamily);
    igcPlatform->SetPlatformType(platform->ePlatformType);
    igcPlatform->SetDeviceID(platform->usDeviceID);
    igcPlatform->SetRevId(platform->usRevId);
    igcPlatform->SetDeviceID_PCH(platform->usDeviceID_PCH);
    igcPlatform->SetRevId_PCH(platform->usRevId_PCH);
    //igcPlatform->SetGTType(platform->eGTType);
    igcPlatform->SetGTType(GTTYPE::GTTYPE_GT3);
    //uint64_t fam = igcPlatform->GetProductFamily();
    //uint64_t core = igcPlatform->GetRenderCoreFamily();
    //printf("fam: %lu, %lu\n", fam, core);
}

void Kernel::TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo) {
    igcGetSystemInfo->SetEuCount(gtSystemInfo->EUCount);
    igcGetSystemInfo->SetThreadCount(gtSystemInfo->ThreadCount);
    igcGetSystemInfo->SetSliceCount(gtSystemInfo->SliceCount);
    igcGetSystemInfo->SetSubSliceCount(gtSystemInfo->SubSliceCount);
    igcGetSystemInfo->SetL3CacheSizeInKb(gtSystemInfo->L3CacheSizeInKb);
    igcGetSystemInfo->SetLLCCacheSizeInKb(gtSystemInfo->LLCCacheSizeInKb);
    //igcGetSystemInfo->SetEdramSizeInKb(gtSystemInfo->EdramSizeInKb);
    igcGetSystemInfo->SetEdramSizeInKb(65536);
    igcGetSystemInfo->SetL3BankCount(gtSystemInfo->L3BankCount);
    igcGetSystemInfo->SetMaxFillRate(gtSystemInfo->MaxFillRate);
    igcGetSystemInfo->SetEuCountPerPoolMax(gtSystemInfo->EuCountPerPoolMax);
    igcGetSystemInfo->SetEuCountPerPoolMin(gtSystemInfo->EuCountPerPoolMin);
    igcGetSystemInfo->SetTotalVsThreads(gtSystemInfo->TotalVsThreads);
    igcGetSystemInfo->SetTotalHsThreads(gtSystemInfo->TotalHsThreads);
    igcGetSystemInfo->SetTotalDsThreads(gtSystemInfo->TotalDsThreads);
    igcGetSystemInfo->SetTotalGsThreads(gtSystemInfo->TotalGsThreads);
    igcGetSystemInfo->SetTotalPsThreadsWindowerRange(gtSystemInfo->TotalPsThreadsWindowerRange);
    igcGetSystemInfo->SetCsrSizeInMb(gtSystemInfo->CsrSizeInMb);
    igcGetSystemInfo->SetMaxEuPerSubSlice(gtSystemInfo->MaxEuPerSubSlice);
    igcGetSystemInfo->SetMaxSlicesSupported(gtSystemInfo->MaxSlicesSupported);
    igcGetSystemInfo->SetMaxSubSlicesSupported(gtSystemInfo->MaxSubSlicesSupported);
    igcGetSystemInfo->SetIsL3HashModeEnabled(gtSystemInfo->IsL3HashModeEnabled);
    igcGetSystemInfo->SetIsDynamicallyPopulated(gtSystemInfo->IsDynamicallyPopulated);
}

void Kernel::TransferFeaturesInfo(IgcFeaturesAndWorkarounds* igcFeWa, FeatureTable* featureTable) {
    /*
    igcFeWa->SetFtrDesktop(featureTable->flags.ftrDesktop);
    igcFeWa->SetFtrChannelSwizzlingXOREnabled(featureTable->flags.ftrChannelSwizzlingXOREnabled);
    igcFeWa->SetFtrGtBigDie(featureTable->flags.ftrGtBigDie);
    igcFeWa->SetFtrGtMediumDie(featureTable->flags.ftrGtMediumDie);
    igcFeWa->SetFtrGtSmallDie(featureTable->flags.ftrGtSmallDie);
    igcFeWa->SetFtrGT1(featureTable->flags.ftrGT1);
    igcFeWa->SetFtrGT1_5(featureTable->flags.ftrGT1_5);
    igcFeWa->SetFtrGT2(featureTable->flags.ftrGT2);
    igcFeWa->SetFtrGT3(featureTable->flags.ftrGT3);
    igcFeWa->SetFtrGT4(featureTable->flags.ftrGT4);
    igcFeWa->SetFtrIVBM0M1Platform(featureTable->flags.ftrIVBM0M1Platform);
    igcFeWa->SetFtrGTL(featureTable->flags.ftrGT1);
    igcFeWa->SetFtrGTM(featureTable->flags.ftrGT2);
    igcFeWa->SetFtrGTH(featureTable->flags.ftrGT3);
    igcFeWa->SetFtrSGTPVSKUStrapPresent(featureTable->flags.ftrSGTPVSKUStrapPresent);
    igcFeWa->SetFtrGTA(featureTable->flags.ftrGTA);
    igcFeWa->SetFtrGTC(featureTable->flags.ftrGTC);
    igcFeWa->SetFtrGTX(featureTable->flags.ftrGTX);
    igcFeWa->SetFtr5Slice(featureTable->flags.ftr5Slice);
    */
    igcFeWa->SetFtrGpGpuMidThreadLevelPreempt(featureTable->flags.ftrGpGpuMidThreadLevelPreempt);
    //igcFeWa->SetFtrIoMmuPageFaulting(featureTable->flags.ftrIoMmuPageFaulting);
    igcFeWa->SetFtrWddm2Svm(featureTable->flags.ftrWddm2Svm);
    igcFeWa->SetFtrPooledEuEnabled(featureTable->flags.ftrPooledEuEnabled);
    //igcFeWa->SetFtrResourceStreamer(featureTable->flags.ftrResourceStreamer);
}

IgcOclTranslationCtx* Kernel::createIgcTranslationCtx() {
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(igcMain, interfaceID, interfaceVersion);
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    int outProfilingTimerResolution = 83;
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    uint64_t platformID = 1;
    uint64_t gtsystemID = 3;
    uint64_t featureID = 2;
    auto igcPlatform = newDeviceCtx->GetPlatformHandleImpl(platformID);
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandleImpl(gtsystemID);
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandleImpl(featureID);
    if (!igcPlatform || !igcGetSystemInfo || !igcFeWa) {
        return nullptr;
    }
    TransferPlatformInfo(igcPlatform, descriptor->pHwInfo->platform);
    TransferSystemInfo(igcGetSystemInfo, descriptor->pHwInfo->gtSystemInfo);
    TransferFeaturesInfo(igcFeWa, descriptor->pHwInfo->featureTable);
    
    uint64_t translationCtxVersion = 3;
    return newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, intermediateType, outType);
}

int Kernel::initialize() {
    int retFcl = loadCompiler(fclName, &fclMain);
    int retIgc = loadCompiler(igcName, &igcMain);
    if (retFcl)
        return retFcl;
    if (retIgc)
        return retIgc;
    return SUCCESS;
}

int Kernel::build(uint16_t chipset_id) {
    int ret = loadProgramSource();
    if (ret)
        return ret;
    if (chipset_id) {
        descriptor = context->device->getDevInfoFromDescriptorTable(chipset_id);
    }
    else {
        descriptor = std::make_unique<DeviceDescriptor>();
        DeviceDescriptor* dd = context->device->getDeviceDescriptor();
        descriptor->pHwInfo = dd->pHwInfo;
        descriptor->setupHardwareInfo = dd->setupHardwareInfo;
        descriptor->devName = dd->devName;
    }
    IgcBuffer* src = CreateIgcBuffer(igcMain, srcCode, srcSize+1);
    IgcBuffer* buildOptions = CreateIgcBuffer(igcMain, options, optionsSize+1);
    printf("options: %s\n", options);

    const char* internal_options = "-ocl-version=300 -cl-disable-zebin -cl-intel-has-buffer-offset-arg -D__IMAGE_SUPPORT__=1 -fpreserve-vec3-type -cl-ext=-all,+cl_khr_byte_addressable_store,+cl_khr_fp16,+cl_khr_global_int32_base_atomics,+cl_khr_global_int32_extended_atomics,+cl_khr_icd,+cl_khr_local_int32_base_atomics,+cl_khr_local_int32_extended_atomics,+cl_intel_command_queue_families,+cl_intel_subgroups,+cl_intel_required_subgroup_size,+cl_intel_subgroups_short,+cl_khr_spir,+cl_intel_accelerator,+cl_intel_driver_diagnostics,+cl_khr_priority_hints,+cl_khr_throttle_hints,+cl_khr_create_command_queue,+cl_intel_subgroups_char,+cl_intel_subgroups_long,+cl_khr_il_program,+cl_intel_mem_force_host_memory,+cl_khr_subgroup_extended_types,+cl_khr_subgroup_non_uniform_vote,+cl_khr_subgroup_ballot,+cl_khr_subgroup_non_uniform_arithmetic,+cl_khr_subgroup_shuffle,+cl_khr_subgroup_shuffle_relative,+cl_khr_subgroup_clustered_reduce,+cl_intel_device_attribute_query,+cl_khr_suggested_local_work_size,+cl_khr_fp64,+cl_khr_subgroups,+cl_intel_spirv_device_side_avc_motion_estimation,+cl_intel_spirv_media_block_io,+cl_intel_spirv_subgroups,+cl_khr_spirv_no_integer_wrap_decoration,+cl_intel_unified_shared_memory_preview,+cl_khr_mipmap_image,+cl_khr_mipmap_image_writes,+cl_intel_planar_yuv,+cl_intel_packed_yuv,+cl_intel_motion_estimation,+cl_intel_device_side_avc_motion_estimation,+cl_intel_advanced_motion_estimation,+cl_khr_int64_base_atomics,+cl_khr_int64_extended_atomics,+cl_khr_image2d_from_buffer,+cl_khr_depth_images,+cl_khr_3d_image_writes,+cl_intel_media_block_io,+cl_intel_va_api_media_sharing,+cl_intel_sharing_format_query,+cl_khr_pci_bus_info";
    size_t internalOptionsSize = strlen(internal_options)+1;
    IgcBuffer* internalOptions = CreateIgcBuffer(igcMain, internal_options, internalOptionsSize);
    printf("internalOptionsSize: %lu\n", internalOptionsSize);
    IgcBuffer* idsBuffer = CreateIgcBuffer(igcMain, nullptr, 0);
    IgcBuffer* valuesBuffer = CreateIgcBuffer(igcMain, nullptr, 0);

    FclOclTranslationCtx* fclTranslationCtx = createFclTranslationCtx();
    auto fclOutput = fclTranslationCtx->TranslateImpl(1, src, buildOptions, internalOptions, nullptr, 0);
    if (fclOutput == nullptr) {
        printf("FCL Compiler error!\n");
        return -1;
    }
    if (fclOutput->Successful() == true) {
        printf("Frontend build success!\n");
    }
    IgcBuffer* fclBuildLog = fclOutput->GetBuildLogImpl(1);
    const char* fclBuildLogMem = reinterpret_cast<const char*>(fclBuildLog->GetMemoryRaw());
    if (fclBuildLogMem) {
        printf("%s\n", fclBuildLogMem);
    }
    IgcBuffer* fclBuildOutput = fclOutput->GetOutputImpl(1);
    const char* fclBuildOutputMem = reinterpret_cast<const char*>(fclBuildOutput->GetMemoryRaw());
    if (fclBuildOutputMem) {
        printf("%s\n", fclBuildOutputMem);
    }
    fclBuildOutput->Retain();
    IgcOclTranslationCtx* igcTranslationCtx = createIgcTranslationCtx();
    if (igcTranslationCtx) {
        printf("igcTranslationCtx creation successful!\n");
    }
    void* gtpinInit = nullptr;
    auto igcOutput = igcTranslationCtx->TranslateImpl(1, fclBuildOutput, idsBuffer, valuesBuffer, buildOptions, internalOptions, nullptr, 0, gtpinInit);
    if (igcOutput == nullptr) {
        printf("IGC Compiler error!\n");
    }
    if (igcOutput->Successful() == true) {
        printf("Backend build successful!\n");
    }
    IgcBuffer* igcBuildLog = igcOutput->GetBuildLogImpl(1);
    const char* igcBuildLogMem = reinterpret_cast<const char*>(igcBuildLog->GetMemoryRaw());
    if (igcBuildLogMem) {
        printf("%s\n", igcBuildLogMem);
    }
    IgcBuffer* igcBuildOutput = igcOutput->GetOutputImpl(1);
    deviceBinary = reinterpret_cast<const char*>(igcBuildOutput->GetMemoryRaw());
    if (deviceBinary) {
        printf("%s\n", deviceBinary);
    }

    return SUCCESS;
}

void Kernel::decodeToken(const PatchItemHeader* token, KernelFromPatchtokens* kernelData) {
    switch (token->Token) {
        case PATCH_TOKEN_SAMPLER_STATE_ARRAY:
            kernelData->samplerStateArray = reinterpret_cast<const PatchSamplerStateArray*>(token);
            break;
        case PATCH_TOKEN_BINDING_TABLE_STATE:
            kernelData->bindingTableState = reinterpret_cast<const PatchBindingTableState*>(token);
            break;
        case PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD:
            kernelData->mediaInterfaceDescriptorLoad = reinterpret_cast<const PatchMediaInterfaceDescriptorLoad*>(token);
            break;
        case PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA:
            kernelData->interfaceDescriptorData = reinterpret_cast<const PatchInterfaceDescriptorData*>(token);
            break;
        case PATCH_TOKEN_EXECUTION_ENVIRONMENT:
            kernelData->executionEnvironment = reinterpret_cast<const PatchExecutionEnvironment*>(token);
            break;
        case PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO:
            kernelData->kernelAttributesInfo = reinterpret_cast<const PatchKernelAttributesInfo*>(token);
            break;
        default:
            break;
    }
}

int Kernel::extractMetadata() {
    // The following usage of reinterpret_cast could lead to undefined behaviour. Checking the header magic
    // makes sure that the reinterpreted memory has the correct format
    const ProgramBinaryHeader* binHeader = reinterpret_cast<const ProgramBinaryHeader*>(deviceBinary);
    if (binHeader->Magic != 0x494E5443) {
        printf("Binary header is wrong!\n");
        return -1;
    }
    //printf("header Magic: %p\n", (void*)(binHeader->Magic));
    //printf("header NumberOfkernels: %u\n", binHeader->NumberOfKernels);
    header = reinterpret_cast<const uint8_t*>(binHeader);
    patchListBlob = header + sizeof(ProgramBinaryHeader);
    kernelInfoBlob = patchListBlob + binHeader->PatchListSize;
    
    kernelData.header = reinterpret_cast<const KernelBinaryHeader*>(kernelInfoBlob);
    //printf("header.Checksum: %u\n", kernelData->header->CheckSum);
    //printf("header.ShaderHashCode: %lu\n", kernelData->header->ShaderHashCode);
    //printf("header.KernelNameSize: %u\n", kernelData->header->KernelNameSize);
    //printf("header.PatchListSize: %u\n", kernelData->header->PatchListSize);
    //printf("header.KernelHeapSize: %u\n", kernelData->header->KernelHeapSize);
    kernelData.kernelInfo = kernelInfoBlob;
    kernelData.name = kernelInfoBlob + sizeof(KernelBinaryHeader);
    //printf("kernel name: %s\n", *(kernelData->name));
    kernelData.isa = kernelData.name + kernelData.header->KernelNameSize;
    kernelData.generalState = kernelData.isa + kernelData.header->KernelHeapSize;
    kernelData.dynamicState = kernelData.generalState + kernelData.header->GeneralStateHeapSize;
    kernelData.surfaceState = kernelData.dynamicState + kernelData.header->DynamicStateHeapSize;
    kernelData.patchList = kernelData.surfaceState + kernelData.header->SurfaceStateHeapSize;
    kernelData.patchListEnd = kernelData.patchList + kernelData.header->PatchListSize;
    const uint8_t* decodePos = kernelData.patchList;
    while (static_cast<uint64_t>(kernelData.patchListEnd - decodePos) > sizeof(PatchItemHeader)) {
        const PatchItemHeader* token = reinterpret_cast<const PatchItemHeader*>(decodePos);
        decodeToken(token, &kernelData);
        decodePos = decodePos + token->Size;
    }
    if (kernelData.bindingTableState == nullptr) {
        return -1;
    }
    printf("bindingTableState->Count = %u\n", kernelData.bindingTableState->Count);
    return SUCCESS;
}

int Kernel::createSipKernel() {
    /*
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(igcMain, interfaceID, interfaceVersion);
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID2 = ;
    uint64_t interfaceVersion = ;
    ICIF* RoutineBufferCtx = CreateInterface(igcMain, interfaceID2, interfaceVersion2);
    IgcBuffer* systemRoutineBuffer = static_cast<IgcBuffer*>(RoutineBufferCtx);
    if (systemRoutineBuffer == nullptr) {
        return nullptr;
    }
    bool result = DeviceCtx->GetSystemRoutine(0u, bindlessSip, systemRoutineBuffer, stateSaveAreaBuffer);    
    */

    return SUCCESS;
}

int Kernel::disassembleBinary() {
    packDeviceBinary();
    return SUCCESS;
}

struct ElfSectionHeader {
    uint32_t name = 0u;
    uint32_t type = SHT_NULL;
    uint64_t flags = SHF_NONE;
    uint64_t addr = 0u;
    uint64_t offset = 0u;
    uint64_t size = 0u;
    uint32_t link = SHN_UNDEF;
    uint32_t info = 0u;
    uint64_t addralign = 0u;
    uint64_t entsize = 0u;
};

struct ElfFileHeader {
    uint16_t type = ET_NONE;
    uint16_t machine = EM_NONE;
    uint32_t version = 1u;
    uint64_t entry = 0u;
    uint64_t phOff = 0u;
    uint64_t shOff = 0u;
    uint32_t flags = 0u;
    uint16_t ehSize = sizeof(ElfFileHeader);
    uint16_t phEntSize = sizeof(ElfProgramHeader);
    uint16_t phNum = 0u;
    uint16_t shEntSize = sizeof(ElfSectionHeader);
    uint16_t shNum = 0u;
    uint16_t shStrNdx = SHN_UNDEF;
};

struct ElfProgramHeader {
    uint32_t type = PT_NULL;
    uint32_t flags = PF_NONE;
    uint64_t offset = 0u;
    uint64_t vAddr = 0u;
    uint64_t pAddr = 0u;
    uint64_t fileSz = 0u;
    uint64_t memSz = 0u;
    uint64_t align = 1u;
};

/*
this->elfFileHeader
this->programHeaders
this->sectionHeaders
data
defaultDataAlignment
stringTable
specialStringsOffsets
maxDataAlignmentNeeded
programSectionLookupTable
*/


// elf_encoder.cpp
std::vector<uint8_t> Kernel::encodeElf() {
    ElfFileHeader elfFileHeader = this->elfFileHeader;
    ElfProgramHeader programHeaders[32] = this->programHeaders;
    ElfSectionHeader sectionHeaders[32] = this->sectionHeaders;
    ElfSectionHeader sectionHeaderNamesSection;
    size_t alignedSectionNamesDataSize = 0u;
    size_t dataPaddingBeforeSectionNames = 0u;
    
    auto alignedDataSize = alignUp(data.size(), static_cast<size_t>(defaultDataAlignment));
    dataPaddingBeforeSectionNames = alignedDataSize - data.size();
    sectionHeaderNamesSection.type = 3u;
    sectionHeaderNamesSection.name = specialStringsOffsets.shStrTab;
    sectionHeaderNamesSection.offset = static_cast<uint64_t>(alignedDataSize);
    sectionHeaderNamesSection.size = static_cast<uint64_t>(stringTable.size());
    sectionHeaderNamesSection.addralign = static_cast<uint64_t>(defaultDataAlignment);
    elfFileHeader.shStrNdx = static_cast<uint16_t>(sectionHeaders.size());
    sectionHeaders[0] = sectionHeaderNamesSection;
    alignedSectionNamesDataSize = alignUp(stringTable.size(), static_cast<size_t>(sectionHeaderNamesSection.addralign));

    elfFileHeader.phNum = static_cast<uint16_t>(programHeaders.size());
    elfFileHeader.shNum = static_cast<uint16_t>(sectionHeaders.size());

    auto programHeadersOffset = elfFileHeader.ehSize;
    auto sectionHeadersOffset = programHeadersOffset + elfFileHeader.phEntSize * elfFileHeader.phNum;
    elfFileHeader.phOff = static_cast<uint64_t>(programHeadersOffset);
    elfFileHeader.shOff = static_cast<uint64_t>(sectionHeadersOffset);
    auto dataOffset = alignUp(sectionHeadersOffset + elfFileHeader.shEntSize * elfFileHeader.shNum, static_cast<size_t>(maxDataAlignmentNeeded));
    auto stringTabOffset = dataOffset + data.size();


    // construct the ELF file
    std::vector<uint8_t> ret;
    ret.reserve(stringTabOffset + alignedSectionNamesDataSize);
    ret.reserve(ret.end(), reinterpret_cast<uint8_t>(&elfFileHeader), reinterpret_cast<uint8_t*>(&elfFileHeader + 1));
    ret.resize(programheadersOffset, 0u);

    for (auto &progSecLookup : programSectionLookupTable) {
        programHeaders[progSecLookup.programId].offset = sectionHeaders[progSecLookup.sectionId].offset;
        programHeaders[progSecLookup.programId].fileSz = sectionHeaders[progSecLookup.sectionId].size;
    }

    for (auto &programHeader : programHeaders) {
        if (0 != programHeader.fileSz) {
            programHeader.offset = static_cast<uint64_t>(programHeader.offset + dataOffset);
        }
        ret.insert(ret.end(), reinterpret_cast<uint8_t*>(&programHeader), reinterpret_cast<uint8_t*>(&programHeader + 1));
        ret.resize(ret.size() + elfFileHeader.phEntSize - sizeof(programHeader), 0u);
    }

    for (auto &sectionHeader : sectionHeaders) {
        if ((8 != sectionHeader.type) && (0 != sectionHeader.size)) {
            sectionHeader.offset = static_cast<uint64_t>(sectionHeader.offset + dataOffset);
        }
        ret.insert(ret.end(), reinterpret_cast<uint8_t*>(&sectionHeader), reinterpret_cast<uint8_t*>(&sectionHeader + 1));
        ret.resize(ret.size() + elfFileHeader.shEntSize - sizeof(sectionHeader), 0u);
    }

    ret.resize(dataOffset, 0u);
    ret.insert(ret.end(), data.begin(), data.end());
    ret.resize(ret.size() + dataPaddingBeforeSectionNames, 0u);
    ret.insert(ret.end(), reinterpret_cast<const uint8_t*>(stringTable.data()), reinterpret_cast<const uint8_t*>(stringTable.data() + static_cast<size_t>(sectionHeaderNamesSection.size)));
    ret.resize(ret.size() + alignedSectionNamesDataSize - static_cast<size_t>(sectionHeaderNamesSection.size), 0u);

    return ret;
}









