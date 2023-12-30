#include "Include\Algorithm\Common.hlsli"

#define BLOCK_SIZE 8

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
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
};

struct GenerateCube
{
	float2 texelSize;	// 1.0 / OutMip1.Dimensions
};

struct ComputeShaderInput
{
	uint3 groupID			: SV_GroupID;			// 3D index of the thread group in the dispatch.
	uint3 groupThreadID		: SV_GroupThreadID;		// 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID	: SV_DispatchThreadID;	// 3D index of global thread ID in the dispatch.
	uint groupIndex			: SV_GroupIndex;		// Flattened local index of the thread within a thread group.
};

static float2 invAtan = float2(0.1591f, 0.3182f);
float2 SampleSphereicalMap(float3 tc)
{
	float2 uv = float2(atan2(tc.z, -tc.x), asin(-tc.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(ComputeShaderInput In)
{
	Texture2D<float4> inputMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2DArray<float4> outTarget = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	ConstantBuffer<GenerateCube> generateCubeCB = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant1Index)];
	
	float2 uv = generateCubeCB.texelSize * (In.dispatchThreadID.xy + 0.5f) - 0.5f;
	
	float3 pos = float3(0.5f, -uv.y, -uv.x);	
	outTarget[uint3(In.dispatchThreadID.xy, 0u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
	
	pos = float3(-0.5f, -uv.y, uv.x);
	outTarget[uint3(In.dispatchThreadID.xy, 1u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
	
	pos = float3(uv.x, 0.5f, uv.y);
	outTarget[uint3(In.dispatchThreadID.xy, 2u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
	
	pos = float3(uv.x, -0.5f, -uv.y);
	outTarget[uint3(In.dispatchThreadID.xy, 3u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
	
	pos = float3(uv.x, -uv.y, 0.5f);
	outTarget[uint3(In.dispatchThreadID.xy, 4u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
	
	pos = float3(-uv.x, -uv.y, -0.5f);
	outTarget[uint3(In.dispatchThreadID.xy, 5u)] = SampleTexture(inputMap, samplerStandard, SampleSphereicalMap(normalize(pos)));
}