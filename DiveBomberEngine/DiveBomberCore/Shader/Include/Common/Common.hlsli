// CommonBuffers
struct RenderInfo
{
	double time;
	uint lightCount;
	float4 screenInfo;
	
	//matrix EVRotation;
	//bool TAA;
	//bool HBAO;
	//bool HDR;
	//float GIScale;
	//bool volumetricRendering;
	//bool skyRendering;
};
ConstantBuffer<RenderInfo> RenderInfoCB : register(b0);

struct CameraInfo
{
	matrix matrix_V;
	matrix matrix_P;
	matrix matrix_VP;
	matrix matrix_I_V;
	matrix matrix_I_P;
	matrix matrix_I_VP;
	
	float3 cameraPos;
	float3 cameraDir;
	float2 cameraFNPlane;
	
	//float4 vWBasisX;
	//float4 vWBasisY;
	//float4 vWBasisZ;
	//float2 UVToViewA;
	//float2 UVToViewB;
};
ConstantBuffer<CameraInfo> CameraInfoCB : register(b1);

struct VertexDataIndex
{
	uint vertexDataIndex;
};
ConstantBuffer<VertexDataIndex> VertexDataIndexCB : register(b2);

struct ModelTransfom
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
	matrix matrix_I_MVP;
};
ConstantBuffer<ModelTransfom> ModelTransfomCB : register(b3);

struct DirectionalLight
{
	float3 direction;
	float3 DdiffuseColor;
	float DdiffuseIntensity;
};
ConstantBuffer<DirectionalLight> DirectionalLightCB : register(b4);

struct AdditionalLight
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

struct AdditionalLightDataIndex
{
	uint AdditionalLightCount;
	uint AdditionalLightDataIndex[3];
};
ConstantBuffer<AdditionalLightDataIndex> AdditionalLightDataIndexCB : register(b5);

struct ShadowInfo
{
	matrix shadowMatrix_VP;
	
	int pcfLevel;
	float depthBias;
	bool hwPcf;
	float cubeShadowBaseOffset;
};
ConstantBuffer<ShadowInfo> ShadowInfoCB : register(b6);

//Shader implement
ConstantBuffer<MaterialIndex> MaterialIndexCB : register(b7);

//Samplers
SamplerState samplerStandard		: register(s0);
SamplerState samplerPoint			: register(s1);
SamplerState samplerStandardClamp	: register(s2);
SamplerState samplerPointClamp		: register(s3);
SamplerState samplerStandardMirror	: register(s4);
SamplerState samplerPointMirror		: register(s5);