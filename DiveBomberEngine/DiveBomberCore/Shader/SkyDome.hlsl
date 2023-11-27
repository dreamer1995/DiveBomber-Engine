#include "Include\header.hlsli"

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

struct VertexDataIndex
{
	uint vertexDataIndex;
};

struct CameraTransforms
{
	matrix matrix_V;
	matrix matrix_P;
	matrix matrix_VP;
	matrix matrix_I_V;
	matrix matrix_I_P;
	matrix matrix_I_VP;
};

struct ModelTransfoms
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
	matrix matrix_I_MVP;
};

struct BaseShadingParams
{
	float4 baseColor;
};

struct VSIn
{
	float3 pos;
	float3 n;
	float3 t;
	float3 b;
	float2 uv;
};

struct ProcessData
{
	float3 uv : Texcoord;
	float4 hPos : SV_Position;
};

ConstantBuffer<CameraTransforms> CameraTransformsCB : register(b0);
ConstantBuffer<VertexDataIndex> LightingDataIndexCB : register(b1);
ConstantBuffer<VertexDataIndex> VertexDataIndexCB : register(b2);
ConstantBuffer<ModelTransfoms> ModelTransfomsCB : register(b3);
ConstantBuffer<MaterialIndex> MaterialIndexCB : register(b4);
SamplerState samp : register(s0);

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
	
	Out.hPos = mul(float4(VSIn[vertexID].pos, 1.0f), ModelTransfomsCB.matrix_MVP).xyww;
	Out.uv = VSIn[vertexID].pos;

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParams> baseShadingParamsCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure0Index)];
	
	float2 skyDomeUV = SampleSphereicalMap(normalize(In.uv));
	
	float4 baseColor = baseMap.SampleLevel(samp, skyDomeUV, 0);
	
	baseColor.rgb = pow(baseColor.rgb, 2.2f);
	
	float4 color = baseColor * baseShadingParamsCB0.baseColor;
	
	return color;
}