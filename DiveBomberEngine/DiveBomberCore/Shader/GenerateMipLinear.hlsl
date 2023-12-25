/**
 * Compute shader to generate mipmaps for a given texture.
 * Source: https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/GenerateMipsCS.hlsli
 */

#define BLOCK_SIZE 8

 // When reducing the size of a texture, it could be that downscaling the texture 
 // will result in a less than exactly 50% (1/2) of the original texture size.
 // This happens if either the width, or the height (or both) dimensions of the texture
 // are odd. For example, downscaling a 5x3 texture will result in a 2x1 texture which
 // has a 60% reduction in the texture width and 66% reduction in the height.
 // When this happens, we need to take more samples from the source texture to 
 // determine the pixel value in the destination texture.

#define WIDTH_HEIGHT_EVEN 0     // Both the width and the height of the texture are even.
#define WIDTH_ODD_HEIGHT_EVEN 1 // The texture width is odd and the height is even.
#define WIDTH_EVEN_HEIGHT_ODD 2 // The texture width is even and teh height is odd.
#define WIDTH_HEIGHT_ODD 3      // Both the width and height of the texture are odd.

"Properties"
{
	"Stage":[ "CS" ],
	"Param":
	[
		{"Name":"baseColor", "Type":"Color", "sRGB":true, "Default":[1.0, 0.0, 1.0, 1.0]}
	]
}
"/Properties"

struct MaterialIndex
{
	uint constant0Index;
	uint constant1Index;
	uint texture0Index;
	uint texture1Index;
	uint texture2Index;
	uint texture3Index;
	uint texture4Index;
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
};

struct GenerateMips
{
	uint srcMipLevel;	// Texture level of source mip
	uint numMipLevels;	// Number of OutMips to write: [1-4]
	uint srcDimension;	// Width and height of the source texture are even or odd.
	bool isSRGB; 		// Must apply gamma correction to sRGB textures.
	float2 texelSize;	// 1.0 / OutMip1.Dimensions
};

struct ComputeShaderInput
{
	uint3 groupID : SV_GroupID;						// 3D index of the thread group in the dispatch.
	uint3 groupThreadID : SV_GroupThreadID;			// 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID : SV_DispatchThreadID;	// 3D index of global thread ID in the dispatch.
	uint droupIndex : SV_GroupIndex;				// Flattened local index of the thread within a thread group.
};

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

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(ComputeShaderInput In)
{
	Texture2D<float4> mainRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2D<float4> outRT0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	RWTexture2D<float4> outRT1 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture2Index)];
	RWTexture2D<float4> outRT2 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture3Index)];
	RWTexture2D<float4> outRT3 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture4Index)];
	ConstantBuffer<BaseShadingParam> baseShadingParam = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	ConstantBuffer<GenerateMips> generateMipsCB = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant1Index)];
	
	float2 uv = generateMipsCB.texelSize * (In.dispatchThreadID.xy + 0.5) / float2(1280, 720);
	
	float4 Src1 = (float4) 0;
	Src1 = mainRT.SampleLevel(samplerStandard, uv, generateMipsCB.srcMipLevel) * baseShadingParam.baseColor;
		
	outRT0[In.dispatchThreadID.xy] = Src1;
	
	// A scalar (constant) branch can exit all threads coherently.
	if (generateMipsCB.numMipLevels == 1)
		return;
	
	// This guarantees all LDS writes are complete and that all threads have
    // executed all instructions so far (and therefore have issued their LDS
    // write instructions.)
	GroupMemoryBarrierWithGroupSync();
	
	outRT1[In.dispatchThreadID.xy / 2] = float4(0.3f, 0.3f, 0.3f, 1.0f);
	
	if (generateMipsCB.numMipLevels == 2)
		return;
	
	GroupMemoryBarrierWithGroupSync();
	
	outRT2[In.dispatchThreadID.xy / 4] = float4(0.2f, 0.2f, 0.2f, 1.0f);
	
	if (generateMipsCB.numMipLevels == 3)
		return;
	
	GroupMemoryBarrierWithGroupSync();
	
	outRT3[In.dispatchThreadID.xy / 8] = float4(0.1f, 0.1f, 0.1f, 1.0f);
}