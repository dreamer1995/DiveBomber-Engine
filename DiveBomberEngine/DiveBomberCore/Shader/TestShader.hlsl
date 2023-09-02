#include "header.hlsli"

struct IndexConstant
{
	uint transform0Index;
	uint transform1Index;
	uint transform2Index;
	uint texure0Index;
	uint texure1Index;
};

struct ModelViewProjection
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_V;
	matrix matrix_P;
	matrix matrix_VP;
	matrix matrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
	matrix matrix_I_V;
	matrix matrix_I_P;
	matrix matrix_I_VP;
	matrix matrix_I_MVP;
};

struct BaseShadingParams
{
	float3 baseColor;
};

struct VSIn
{
    float3 pos : Position;
    float3 n : Normal;
    float3 t : Tangent;
    float3 b : Binormal;
    float2 uv : Texcoord;
};

struct ProcessData
{
    float2 uv	: Texcoord;
    float4 hPos : SV_Position;
};

ConstantBuffer<IndexConstant> IndexConstantCB : register(b0);
SamplerState samp : register(s0);

ProcessData VSMain(VSIn In)
{
	ProcessData Out;

	ConstantBuffer<ModelViewProjection> ModelViewProjectionCB = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.transform0Index)];
	
    Out.hPos = mul(float4(In.pos, 1.0f), ModelViewProjectionCB.matrix_MVP);
    Out.uv = In.uv;

    return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParams> BaseShadingParamsCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.transform1Index)];
	ConstantBuffer<BaseShadingParams> BaseShadingParamsCB1 = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.transform2Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texure0Index)];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texure1Index)];
	
	float4 baseColor = baseMap.Sample(samp, In.uv);
	float4 rustColor = rustMap.Sample(samp, In.uv);
	
	baseColor.rgb = pow(baseColor.rgb, 2.2f);
	rustColor.rgb = pow(rustColor.rgb, 2.2f);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g) * (float4(BaseShadingParamsCB0.baseColor, 1.0f) + float4(BaseShadingParamsCB1.baseColor, 1.0f));
	
	color.rgb = pow(color.rgb, 1 / 2.2f);
	
	return color;
}