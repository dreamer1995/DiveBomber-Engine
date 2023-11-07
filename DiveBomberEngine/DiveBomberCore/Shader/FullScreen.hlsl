#include "Include\header.hlsli"

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

struct BaseShadingParams
{
	float4 baseColor;
};

struct VSIn
{
	float2 pos;
};

struct ProcessData
{
	float2 uv : Texcoord;
	float4 hPos : SV_Position;
};

ConstantBuffer<CameraTransforms> CameraTransformsCB : register(b0);
ConstantBuffer<VertexDataIndex> LightingDataIndexCB : register(b1);
ConstantBuffer<VertexDataIndex> VertexDataIndexCB : register(b2);
ConstantBuffer<MaterialIndex> MaterialIndexCB : register(b4);
SamplerState samp : register(s0);

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	float2 pos = VSIn[vertexID].pos;
	Out.hPos = float4(pos, 0.0f, 1.0f);
	Out.uv = float2((pos.x + 1) / 2.0f, -(pos.y - 1) / 2.0f);

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	Texture2D<float4> mainRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	
	float4 col = mainRT.Sample(samp, In.uv);
	
	return col * float4(1, 1, 0, 1);
}