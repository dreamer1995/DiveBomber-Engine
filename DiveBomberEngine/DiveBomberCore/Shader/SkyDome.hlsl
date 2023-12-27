#include "Include\Algorithm\Common.hlsli"

"Properties"
{
	"Stage":[ "VS","PS" ],
	"Param":
	[
		{"Name":"baseMap", "Type":"Texture", "sRGB":true},

		{"Name":"baseColor", "Type":"Color", "sRGB":true, "Default":[1.0, 0.0, 0.0, 1.0]}
	]
}
"/Properties"

struct MaterialIndex
{
	uint constant0Index;
	uint texure0Index;
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
	float3 uv			: Texcoord;
	float4 hPosition	: SV_Position;
};

static float2 invAtan = float2(0.1591f, 0.3182f);
float2 SampleSphereicalMap(float3 tc)
{
	float2 uv = float2(atan2(tc.z, -tc.x), asin(-tc.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	Out.hPosition = mul(float4(VSIn[vertexID].position, 1.0f), ModelTransfomCB.matrix_MVP).xyww;
	Out.uv = VSIn[vertexID].position;

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParam> baseShadingParamCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure0Index)];
	
	float2 skyDomeUV = SampleSphereicalMap(normalize(In.uv));
	
	float4 baseColor = SampleSRGBTextureLevel(baseMap, samplerStandard, skyDomeUV, 0u);
	
	float4 color = baseColor * baseShadingParamCB0.baseColor;
	
	return color;
}