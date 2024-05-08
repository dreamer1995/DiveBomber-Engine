#include "Include\Algorithm\Common.hlsli"

"Properties"
{
	"Stage":[ "CS" ],
	"Param":
	[
		{"Name":"baseColor", "Type":"Color", "Default":[1.0, 1.0, 1.0, 1.0]}
	]
}
"/Properties"

struct PostProcessData
{
	float2 invScreenSize;
};

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

struct ComputeShaderInput
{
	uint3 groupID			: SV_GroupID;			// 3D index of the thread group in the dispatch.
	uint3 groupThreadID		: SV_GroupThreadID;		// 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID	: SV_DispatchThreadID;	// 3D index of global thread ID in the dispatch.
	uint groupIndex			: SV_GroupIndex;		// Flattened local index of the thread within a thread group.
};

[numthreads(8, 8, 1)]
void CSMain(ComputeShaderInput In)
{
	Texture2D<float4> mainRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2D<float4> outRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	ConstantBuffer<BaseShadingParam> baseShadingParam = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	ConstantBuffer<PostProcessData> postProcessData = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant1Index)];
	
	float2 uv = (In.dispatchThreadID.xy + 0.5) * postProcessData.invScreenSize;
	
	float4 col = SampleTexture(mainRT, samplerStandard, uv) * DecodeGamma(baseShadingParam.baseColor);
	
	col.rgb = EncodeGamma(col.rgb);
		
	outRT[In.dispatchThreadID.xy] = col;
}