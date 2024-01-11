#ifndef __AlgorithmMipMapGenerate__
#define __AlgorithmMipMapGenerate__

// The reason for separating channels is to reduce bank conflicts in the
// local data memory controller.  A large stride will cause more threads
// to collide on the same memory bank.
groupshared float gs_R[64];
groupshared float gs_G[64];
groupshared float gs_B[64];
groupshared float gs_A[64];

void StoreColor(uint Index, float4 Color)
{
	gs_R[Index] = Color.r;
	gs_G[Index] = Color.g;
	gs_B[Index] = Color.b;
	gs_A[Index] = Color.a;
}

float4 LoadColor(uint Index)
{
	return float4(gs_R[Index], gs_G[Index], gs_B[Index], gs_A[Index]);
}

#endif // __AlgorithmMipMapGenerate__