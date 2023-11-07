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
	float4 hPos : SV_Position;
};

ConstantBuffer<CameraTransforms> CameraTransformsCB : register(b0);
ConstantBuffer<VertexDataIndex> LightingDataIndexCB : register(b1);
ConstantBuffer<VertexDataIndex> VertexDataIndexCB : register(b2);
ConstantBuffer<ModelTransfoms> ModelTransfomsCB : register(b3);
ConstantBuffer<MaterialIndex> MaterialIndexCB : register(b4);

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	Out.hPos = mul(float4(VSIn[vertexID].pos, 1.0f), ModelTransfomsCB.matrix_MVP);

	return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParams> baseShadingParamsCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	return baseShadingParamsCB0.baseColor;
}