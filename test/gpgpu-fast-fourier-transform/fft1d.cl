/* ************************************************************************
 * Copyright 2013 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/



#define fptype float

#define fvect2 float2

#define C8Q  0.70710678118654752440084436210485f

__attribute__((always_inline)) void 
FwdRad4B1(float2 *R0, float2 *R2, float2 *R1, float2 *R3)
{

	float2 T;

	(*R1) = (*R0) - (*R1);
	(*R0) = 2.0f * (*R0) - (*R1);
	(*R3) = (*R2) - (*R3);
	(*R2) = 2.0f * (*R2) - (*R3);
	
	(*R2) = (*R0) - (*R2);
	(*R0) = 2.0f * (*R0) - (*R2);
	(*R3) = (*R1) + (fvect2)(-(*R3).y, (*R3).x);
	(*R1) = 2.0f * (*R1) - (*R3);
	
	T = (*R1); (*R1) = (*R2); (*R2) = T;
	
}


__attribute__((always_inline)) void
FwdPass0(uint rw, uint b, uint me, uint inOffset, uint outOffset, __global float2 *bufIn, __local float *bufOutRe, __local float *bufOutIm, float2 *R0, float2 *R1, float2 *R2, float2 *R3)
{


	if(rw)
	{
	(*R0) = bufIn[inOffset + ( 0 + me*1 + 0 + 0 )*1];
	(*R1) = bufIn[inOffset + ( 0 + me*1 + 0 + 4 )*1];
	(*R2) = bufIn[inOffset + ( 0 + me*1 + 0 + 8 )*1];
	(*R3) = bufIn[inOffset + ( 0 + me*1 + 0 + 12 )*1];
	}

	else
	{
	(*R0) = (fvect2)(0, 0);
	(*R1) = (fvect2)(0, 0);
	(*R2) = (fvect2)(0, 0);
	(*R3) = (fvect2)(0, 0);
	}



	FwdRad4B1(R0, R1, R2, R3);


	if(rw)
	{
	bufOutRe[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 0 )*1] = (*R0).x;
	bufOutRe[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 1 )*1] = (*R1).x;
	bufOutRe[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 2 )*1] = (*R2).x;
	bufOutRe[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 3 )*1] = (*R3).x;
	}


	barrier(CLK_LOCAL_MEM_FENCE);

	if(rw)
	{
	(*R0).x = bufOutRe[outOffset + ( 0 + me*1 + 0 + 0 )*1];
	(*R1).x = bufOutRe[outOffset + ( 0 + me*1 + 0 + 4 )*1];
	(*R2).x = bufOutRe[outOffset + ( 0 + me*1 + 0 + 8 )*1];
	(*R3).x = bufOutRe[outOffset + ( 0 + me*1 + 0 + 12 )*1];
	}


	barrier(CLK_LOCAL_MEM_FENCE);

	if(rw)
	{
	bufOutIm[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 0 )*1] = (*R0).y;
	bufOutIm[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 1 )*1] = (*R1).y;
	bufOutIm[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 2 )*1] = (*R2).y;
	bufOutIm[outOffset + ( ((1*me + 0)/1)*4 + (1*me + 0)%1 + 3 )*1] = (*R3).y;
	}


	barrier(CLK_LOCAL_MEM_FENCE);

	if(rw)
	{
	(*R0).y = bufOutIm[outOffset + ( 0 + me*1 + 0 + 0 )*1];
	(*R1).y = bufOutIm[outOffset + ( 0 + me*1 + 0 + 4 )*1];
	(*R2).y = bufOutIm[outOffset + ( 0 + me*1 + 0 + 8 )*1];
	(*R3).y = bufOutIm[outOffset + ( 0 + me*1 + 0 + 12 )*1];
	}


	barrier(CLK_LOCAL_MEM_FENCE);

}

__attribute__((always_inline)) void
FwdPass1(float2* twiddles, uint rw, uint b, uint me, uint inOffset, uint outOffset, __local float *bufInRe, __local float *bufInIm, __global float2 *bufOut, float2 *R0, float2 *R1, float2 *R2, float2 *R3)
{




	{
		float2 W = twiddles[3 + 3*((1*me + 0)%4) + 0];
		float TR, TI;
		TR = (W.x * (*R1).x) - (W.y * (*R1).y);
		TI = (W.y * (*R1).x) + (W.x * (*R1).y);
		(*R1).x = TR;
		(*R1).y = TI;
	}

	{
		float2 W = twiddles[3 + 3*((1*me + 0)%4) + 1];
		float TR, TI;
		TR = (W.x * (*R2).x) - (W.y * (*R2).y);
		TI = (W.y * (*R2).x) + (W.x * (*R2).y);
		(*R2).x = TR;
		(*R2).y = TI;
	}

	{
		float2 W = twiddles[3 + 3*((1*me + 0)%4) + 2];
		float TR, TI;
		TR = (W.x * (*R3).x) - (W.y * (*R3).y);
		TI = (W.y * (*R3).x) + (W.x * (*R3).y);
		(*R3).x = TR;
		(*R3).y = TI;
	}

	FwdRad4B1(R0, R1, R2, R3);


	if(rw)
	{
	bufOut[outOffset + ( 1*me + 0 + 0 )*1] = (*R0);
	bufOut[outOffset + ( 1*me + 0 + 4 )*1] = (*R1);
	bufOut[outOffset + ( 1*me + 0 + 8 )*1] = (*R2);
	bufOut[outOffset + ( 1*me + 0 + 12 )*1] = (*R3);
	}

}



typedef union {
        uint u;
        int i;
} cb_t;

__kernel __attribute__((reqd_work_group_size (64,1,1)))
void fft_fwd(__constant cb_t *cb, __global float2* restrict gb)
{
	uint me = get_local_id(0);
	uint batch = get_group_id(0);

        float2 twiddles[15] = {
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(1.0000000000000000000000000000000000e+00f, -0.0000000000000000000000000000000000e+00f),
        (float2)(9.2387953251128673848313610506011173e-01f, -3.8268343236508978177923268049198668e-01f),
        (float2)(7.0710678118654757273731092936941423e-01f, -7.0710678118654746171500846685376018e-01f),
        (float2)(3.8268343236508983729038391174981371e-01f, -9.2387953251128673848313610506011173e-01f),
        (float2)(7.0710678118654757273731092936941423e-01f, -7.0710678118654746171500846685376018e-01f),
        (float2)(6.1232339957367660358688201472919830e-17f, -1.0000000000000000000000000000000000e+00f),
        (float2)(-7.0710678118654746171500846685376018e-01f, -7.0710678118654757273731092936941423e-01f),
        (float2)(3.8268343236508983729038391174981371e-01f, -9.2387953251128673848313610506011173e-01f),
        (float2)(-7.0710678118654746171500846685376018e-01f, -7.0710678118654757273731092936941423e-01f),
        (float2)(-9.2387953251128684950543856757576577e-01f, 3.8268343236508967075693021797633264e-01f),
        };

	__local float lds[256];

	uint ioOffset;
	__global float2 *lwb;

	float2 R0, R1, R2, R3;

	uint rw = (me < ((cb[0].u) - batch*16)*4) ? 1 : 0;

	uint b = 0;

	ioOffset = (batch*16 + (me/4))*16;
	lwb = gb + ioOffset;

	FwdPass0(rw, b, me%4, 0, (me/4)*16, lwb, lds, lds, &R0, &R1, &R2, &R3);
	FwdPass1(twiddles, rw, b, me%4, (me/4)*16, 0, lds, lds, lwb, &R0, &R1, &R2, &R3);
}

