#include "Include\Algorithm\Common.hlsli"

"Properties"
{
	"Stage":[ "VS","PS" ],
	"Param":
	[
		{"Name":"BaseMap", "Type":"Texture"},
		{"Name":"RustMap", "Type":"Texture", "Default":"Gray"},

		{"Name":"BaseColor", "Type":"Color", "Default":[1.0, 1.0, 1.0, 1.0]},
		{"Name":"BaseColor2", "Type":"Color", "Default":[0.0, 0.0, 0.0, 1.0]}
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

struct GBufferOutput
{
	float4 GBuffer0		: SV_Target0;	// GBufferA[R32G32B32A32F]	: MainTarget, include Emission.rgb/forward rendered GI
	float4 GBuffer1		: SV_Target1;	// GBufferA[R8G8B8A8]		: BaseColor.rgb, Metallic
	float4 GBuffer2 	: SV_Target2;	// GBufferB[R8G8B8A8]		: Roughness, AO, Specular, InShadow(Obsolete), ShadingModelID
	float4 GBuffer3 	: SV_Target3;	// GBufferC[R10G10B10A2]	: N.xyz, Temp
	float4 GBuffer4 	: SV_Target4;	// GBufferD[R8G8B8A8]		: CustomData
//#if USE_PIXEL_OFFSET
//	float  PixelDepth : SV_Depth;     // For pixel offset
//#endif
};

ProcessData VSMain(uint vertexID : SV_VertexID)
{
	ProcessData Out;
	StructuredBuffer<VSIn> VSIn = ResourceDescriptorHeap[NonUniformResourceIndex(VertexDataIndexCB.vertexDataIndex)];
	
	Out.hPosition = mul(float4(VSIn[vertexID].position, 1.0f), ModelTransfomCB.matrix_MVP);
	Out.uv = VSIn[vertexID].uv;

	return Out;
}

GBufferOutput PSMain(ProcessData In)
{
	GBufferOutput finalOut;
	ConstantBuffer<BaseShadingParam> baseShadingParamCB0 = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.constant0Index)];
	
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure0Index)];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(MaterialIndexCB.texure1Index)];
	
	float4 baseColor = SampleSRGBTexture(baseMap, samplerStandard, In.uv);
	float4 rustColor = SampleSRGBTexture(rustMap, samplerStandard, In.uv);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g) *
	(DecodeGamma(baseShadingParamCB0.baseColor) + DecodeGamma(baseShadingParamCB0.baseColor2));
	
	finalOut.GBuffer0 = color;
	finalOut.GBuffer1 = float4(1, 0, 0, 1);
	finalOut.GBuffer2 = float4(0, 1, 0, 1);
	finalOut.GBuffer3 = float4(0, 0, 1, 1);
	finalOut.GBuffer4 = float4(1, 1, 0, 1);

	return finalOut;
}