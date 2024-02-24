#include "Include\Algorithm\Common.hlsli"
#include "Include\Algorithm\CubeMapGenerate.hlsli"

#define BLOCK_SIZE 8

"Properties"
{
	"Stage":[ "CS" ],
	"Param":
	[
	]
}
"/Properties"

struct MaterialIndex
{
	uint constant0Index;
	uint constant1Index;
	uint texture0Index;
	uint texture1Index;
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
};

struct GenerateDiffuseMips
{
	float roughness;
	bool isSRGB;
	float2 texelSize;
};

struct ComputeShaderInput
{
	uint3 groupID			: SV_GroupID;			// 3D index of the thread group in the dispatch.
	uint3 groupThreadID		: SV_GroupThreadID;		// 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID	: SV_DispatchThreadID;	// 3D index of global thread ID in the dispatch.
	uint groupIndex			: SV_GroupIndex;		// Flattened local index of the thread within a thread group.
};

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(ComputeShaderInput In)
{
	TextureCube<float4> inputMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2DArray<float4> outMip1 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	ConstantBuffer<GenerateDiffuseMips> generateDiffuseMips = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant1Index)];
	
	float2 uv = generateDiffuseMips.texelSize * (In.dispatchThreadID.xy + 0.5f) - 0.5f;
	float3 normalVec = normalize(ThreadIDToCubeFaceCoordinate(uv, In.dispatchThreadID.z));
	
	uint2 random = Rand3DPCG16(uint3(In.dispatchThreadID)).xy;
	
	outMip1[In.dispatchThreadID] = PackColor(
		float4(ConvolutionCubeMapSpecular(inputMap, samplerStandard, random, normalVec,
			generateDiffuseMips.roughness, generateDiffuseMips.isSRGB), 1.0f),
			generateDiffuseMips.isSRGB);
}