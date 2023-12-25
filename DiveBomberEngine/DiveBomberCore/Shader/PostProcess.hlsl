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
	
	float2 uv = (In.dispatchThreadID.xy + 0.5) / float2(1280, 720);
	
	float4 col = mainRT.Sample(samplerStandard, uv) * baseShadingParam.baseColor;
	
	col.rgb = pow(col.rgb, 1 / 2.2f);
		
	outRT[In.dispatchThreadID.xy] = col;
}