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
	uint texture0Index;
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
};

struct VSIn
{
	float3 position;
};

struct ProcessData
{
	float2 uv			: Texcoord;
	float4 hPosition	: SV_Position;
};

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	float2 position = VSIn[vertexID].position;
	Out.hPosition = float4(position, 0.0f, 1.0f);
	Out.uv = float2((position.x + 1) / 2.0f, -(position.y - 1) / 2.0f);

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	Texture2D<float4> mainRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	ConstantBuffer<BaseShadingParam> baseShadingParam = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	float4 col = mainRT.Sample(samplerStandard, In.uv) * baseShadingParam.baseColor;
	
	return col;
}