#include "Include\Algorithm\Common.hlsli"

"Properties"
{
	"Stage":[ "VS","PS" ],
	"Param":
	[
		{"Name":"baseMap", "Type":"Texture", "sRGB":true},
		{"Name":"rustMap", "Type":"Texture", "sRGB":false, "Default":"Gray"},

		{"Name":"baseColor", "Type":"Color", "sRGB":true, "Default":[1.0, 0.0, 0.0, 1.0]},
		{"Name":"baseColor2", "Type":"Color", "sRGB":true, "Default":[0.0, 1.0, 0.0, 1.0]}
	]
}
"/Properties"

struct MaterialIndex
{
	uint constant0Index;
	uint texure0Index;
	uint texure1Index;
};
#include "Include\Common\Common.hlsli"

struct BaseShadingParam
{
	float4 baseColor;
	float4 baseColor2;
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
	float2 uv			: Texcoord;
	float4 hPosition	: SV_Position;
};

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	Out.hPosition = mul(float4(VSIn[vertexID].position, 1.0f), ModelTransfomCB.matrix_MVP);
	Out.uv = VSIn[vertexID].uv;

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParam> baseShadingParamCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure0Index)];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure1Index)];
	
	float4 baseColor = SampleSRGBTexture(baseMap, samplerStandard, In.uv);
	float4 rustColor = SampleSRGBTexture(rustMap, samplerStandard, In.uv);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g) * (baseShadingParamCB0.baseColor + baseShadingParamCB0.baseColor2);
	
	return color;
}