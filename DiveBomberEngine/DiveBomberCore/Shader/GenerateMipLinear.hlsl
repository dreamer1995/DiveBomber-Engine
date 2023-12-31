#include "Include\Algorithm\Common.hlsli"

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
	uint inputMapLevel;	// Texture level of source mip
	uint numMipLevels;	// Number of OutMips to write: [1-4]
	uint srcDimension;	// Width and height of the source texture are even or odd.
	bool isSRGB; 		// Must apply gamma correction to sRGB textures.
	float2 texelSize;	// 1.0 / OutMip1.Dimensions
};

struct ComputeShaderInput
{
	uint3 groupID			: SV_GroupID;			// 3D index of the thread group in the dispatch.
	uint3 groupThreadID		: SV_GroupThreadID;		// 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID	: SV_DispatchThreadID;	// 3D index of global thread ID in the dispatch.
	uint groupIndex			: SV_GroupIndex;		// Flattened local index of the thread within a thread group.
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

float4 LoadColor(uint Index, bool isSRGB)
{
	if (isSRGB)
	{
		return float4(DecodeGamma(float3(gs_R[Index], gs_G[Index], gs_B[Index])), gs_A[Index]);
	}
	else
	{
		return float4(gs_R[Index], gs_G[Index], gs_B[Index], gs_A[Index]);
	}
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

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(ComputeShaderInput In)
{
	Texture2DArray<float4> inputMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2DArray<float4> outMip1 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	RWTexture2DArray<float4> outMip2 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture2Index)];
	RWTexture2DArray<float4> outMip3 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture3Index)];
	RWTexture2DArray<float4> outMip4 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture4Index)];
	ConstantBuffer<GenerateMips> generateMipsCB = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant1Index)];
	
	float4 src1 = (float4) 0;
		
	// One bilinear sample is insufficient when scaling down by more than 2x.
    // You will slightly undersample in the case where the source dimension
    // is odd.  This is why it's a really good idea to only generate mips on
    // power-of-two sized textures.  Trying to handle the undersampling case
    // will force this shader to be slower and more complicated as it will
    // have to take more source texture samples.

    // Determine the path to use based on the dimension of the 
    // source texture.
    // 0b00(0): Both width and height are even.
    // 0b01(1): Width is odd, height is even.
    // 0b10(2): Width is even, height is odd.
    // 0b11(3): Both width and height are odd.
	switch (generateMipsCB.srcDimension)
	{
		case WIDTH_HEIGHT_EVEN:
        {
			float3 uv = float3(generateMipsCB.texelSize * (In.dispatchThreadID.xy + 0.5f), In.dispatchThreadID.z);

			src1 = SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB);
		}
		break;
		case WIDTH_ODD_HEIGHT_EVEN:
        {
			// > 2:1 in X dimension
			// Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
			// horizontally.
			float3 uv1 = float3(generateMipsCB.texelSize * (In.dispatchThreadID.xy + float2(0.25f, 0.5f)), In.dispatchThreadID.z);
			float3 offset = float3(generateMipsCB.texelSize * float2(0.5f, 0.0f), 0.0f);

			src1 = 0.5f * (SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB) +
                        SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1 + offset, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB));
		}
		break;
		case WIDTH_EVEN_HEIGHT_ODD:
        {
            // > 2:1 in Y dimension
            // Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
            // vertically.
			float3 uv1 = float3(generateMipsCB.texelSize * (In.dispatchThreadID.xy + float2(0.5f, 0.25f)), In.dispatchThreadID.z);
			float3 offset = float3(generateMipsCB.texelSize * float2(0.0f, 0.5f), 0.0f);

			src1 = 0.5f * (SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB) +
                        SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1 + offset, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB));
		}
		break;
		case WIDTH_HEIGHT_ODD:
        {
            // > 2:1 in in both dimensions
            // Use 4 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
            // in both directions.
			float3 uv1 = float3(generateMipsCB.texelSize * (In.dispatchThreadID.xy + float2(0.25f, 0.25f)), In.dispatchThreadID.z);
			float2 offset = generateMipsCB.texelSize * 0.5f;

			src1 = SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1, generateMipsCB.inputMapLevel, generateMipsCB.isSRGB);
			src1 += SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1 + float3(0.0f, offset.y, 0.0f), generateMipsCB.inputMapLevel, generateMipsCB.isSRGB);
			src1 += SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1 + float3(0.0f, offset.y, 0.0f), generateMipsCB.inputMapLevel, generateMipsCB.isSRGB);
			src1 += SampleTextureArrayLevel(inputMap, samplerStandardClamp, uv1 + float3(offset.x, offset.y, 0.0f), generateMipsCB.inputMapLevel, generateMipsCB.isSRGB);
			src1 *= 0.25f;
		}
		break;
	}
	
	outMip1[In.dispatchThreadID] = PackColor(src1, generateMipsCB.isSRGB);
	
	// A scalar (constant) branch can exit all threads coherently.
	if (generateMipsCB.numMipLevels == 1)
		return;
	
	// Without lane swizzle operations, the only way to share data with other
    // threads is through LDS.
	StoreColor(In.groupIndex, src1);
	
	// This guarantees all LDS writes are complete and that all threads have
    // executed all instructions so far (and therefore have issued their LDS
    // write instructions.)
	GroupMemoryBarrierWithGroupSync();
	
	// With low three bits for X and high three bits for Y, this bit mask
    // (binary: 001001) checks that X and Y are even.
	if ((In.groupIndex & 0x9) == 0)
	{
		float4 src2 = LoadColor(In.groupIndex + 0x01, generateMipsCB.isSRGB);
		float4 src3 = LoadColor(In.groupIndex + 0x08, generateMipsCB.isSRGB);
		float4 src4 = LoadColor(In.groupIndex + 0x09, generateMipsCB.isSRGB);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		outMip2[uint3(In.dispatchThreadID.xy / 2, In.dispatchThreadID.z)] = PackColor(src1, generateMipsCB.isSRGB);
		StoreColor(In.groupIndex, src1);
	}
	
	if (generateMipsCB.numMipLevels == 2)
		return;
	
	GroupMemoryBarrierWithGroupSync();
	
	// This bit mask (binary: 011011) checks that X and Y are multiples of four.
	if ((In.groupIndex & 0x1B) == 0)
	{
		float4 src2 = LoadColor(In.groupIndex + 0x02, generateMipsCB.isSRGB);
		float4 src3 = LoadColor(In.groupIndex + 0x10, generateMipsCB.isSRGB);
		float4 src4 = LoadColor(In.groupIndex + 0x12, generateMipsCB.isSRGB);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		outMip3[uint3(In.dispatchThreadID.xy / 4, In.dispatchThreadID.z)] = PackColor(src1, generateMipsCB.isSRGB);
		StoreColor(In.groupIndex, src1);
	}
	
	if (generateMipsCB.numMipLevels == 3)
		return;
	
	GroupMemoryBarrierWithGroupSync();
	
	// This bit mask would be 111111 (X & Y multiples of 8), but only one
    // thread fits that criteria.
	if (In.groupIndex == 0)
	{
		float4 src2 = LoadColor(In.groupIndex + 0x04, generateMipsCB.isSRGB);
		float4 src3 = LoadColor(In.groupIndex + 0x20, generateMipsCB.isSRGB);
		float4 src4 = LoadColor(In.groupIndex + 0x24, generateMipsCB.isSRGB);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		outMip4[uint3(In.dispatchThreadID.xy / 8, In.dispatchThreadID.z)] = PackColor(src1, generateMipsCB.isSRGB);
	}
}