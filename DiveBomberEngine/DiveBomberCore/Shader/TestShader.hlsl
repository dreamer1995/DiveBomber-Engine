#include "header.hlsli"

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

struct IndexConstant
{
	uint constant0Index;
	uint constant1Index;
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
	float4 baseColor;
	float4 baseColor2;
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

	ConstantBuffer<ModelViewProjection> ModelViewProjectionCB = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.constant1Index)];
	
    Out.hPos = mul(float4(In.pos, 1.0f), ModelViewProjectionCB.matrix_MVP);
    Out.uv = In.uv;

    return Out;
}

float4 PSMain(ProcessData In) : SV_Target
{
	ConstantBuffer<BaseShadingParams> baseShadingParamsCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.constant0Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texure0Index)];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texure1Index)];
	
	float4 baseColor = baseMap.Sample(samp, In.uv);
	float4 rustColor = rustMap.Sample(samp, In.uv);
	
	baseColor.rgb = pow(baseColor.rgb, 2.2f);
	rustColor.rgb = pow(rustColor.rgb, 2.2f);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g) * (baseShadingParamsCB0.baseColor + baseShadingParamsCB0.baseColor2);
	
	color.rgb = pow(color.rgb, 1 / 2.2f);
	
	return color;
}