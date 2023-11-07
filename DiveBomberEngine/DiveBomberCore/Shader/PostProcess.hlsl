#include "Include\header.hlsli"

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

struct ComputeShaderInput
{
	uint3 GroupID : SV_GroupID; // 3D index of the thread group in the dispatch.
	uint3 GroupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
	uint3 DispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
	uint GroupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

ConstantBuffer<CameraTransforms> CameraTransformsCB : register(b0);
ConstantBuffer<CameraTransforms> LightingDataIndexCB : register(b1);
ConstantBuffer<MaterialIndex> MaterialIndexCB : register(b4);
SamplerState samp : register(s0);

[numthreads(8, 8, 1)]
void CSMain(ComputeShaderInput In)
{
	Texture2D<float4> mainRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture0Index)];
	RWTexture2D<float4> outRT = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texture1Index)];
	ConstantBuffer<BaseShadingParams> baseShadingParams = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	float2 uv = (In.DispatchThreadID.xy + 0.5) / float2(1280, 720);
	
	float4 col = mainRT.Sample(samp, uv) * baseShadingParams.baseColor;
		
	outRT[In.DispatchThreadID.xy] = col;
}