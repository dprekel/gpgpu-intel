#pragma once

struct GPU;

const char* decodeCapabilities(uint64_t capability);
void showTopology(GPU* gpuInfo);
