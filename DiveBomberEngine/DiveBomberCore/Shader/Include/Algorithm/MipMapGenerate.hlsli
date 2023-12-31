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

// Convert linear color to sRGB before storing if the original source is 
// an sRGB texture.
float4 PackColor(float4 x, bool isSRGB)
{
	if (isSRGB)
	{
		return float4(EncodeGamma(x.rgb), x.a);
	}
	else
	{
		return x;
	}
}