#include<stdio.h>
#include<stdlib.h>
#include<CL/cl.h>
#include"api.h"




int main() {
	cl_uint platformCount;
	cl_uint platform_ret;
	
	printf("platformCount pointer is: %p\n", &platformCount);
	platform_ret = clGetPlatformIDs(5, NULL, &platformCount);
	printf("Return Value of clGetPlatformIDs: %d\n", platform_ret);

	return 0;
}
