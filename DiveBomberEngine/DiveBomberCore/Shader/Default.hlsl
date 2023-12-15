"Properties"
{
	"Stage":[ "VS","PS" ],
	"Param":
	[
		{"Name":"baseColor", "Type":"Color", "sRGB":true}
	]
}
"/Properties"

struct MaterialIndex
{
	uint constant0Index;
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
};

struct VSIn
{
	float3 position;
	float3 normal;
	float3 tangent;
	float3 binormal;
	float2 uv;
};

struct ProcessData
{
	float4 hPosition : SV_Position;
};

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	Out.hPosition = mul(float4(VSIn[vertexID].position, 1.0f), ModelTransfomCB.matrix_MVP);

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParam> baseShadingParamCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	return baseShadingParamCB0.baseColor;
}