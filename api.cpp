#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<cstring>
#include<CL/cl.h>
#include"cl_execution_environment.h"





using namespace NEO;

cl_int CL_API_CALL clGetPlatformIDs(cl_uint numEntries,
				    cl_platform_id* platforms,
				    cl_uint* numPlatforms) {
	printf("numPlatforms pointer: %p\n", numPlatforms);
	cl_int retVal = CL_SUCCESS;
	
	if ((platforms == nullptr) && (numPlatforms == nullptr)) {
		retVal = CL_INVALID_VALUE;
	}
	
	if (numEntries == 0 && platforms != nullptr) {
		retVal = CL_INVALID_VALUE;
	}
	
	auto executionEnvironment = new ClExecutionEnvironment();	
	//class ClExecutionEnvironment* executionEnvironment = ClExecutionEnvironment::ClExecutionEnvironment();
	//executionEnvironment->incRefInternal();
	
	//if (platformsImpl->empty()) {
	//	printf("We did it!!!!\n");
	//}
	
	return retVal;
}
