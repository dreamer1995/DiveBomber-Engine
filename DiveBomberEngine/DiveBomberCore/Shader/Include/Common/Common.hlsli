#include "Common.hlsli"

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

cbuffer CameraCBuf : register(b1)//VS, PS
{
	float3 cameraPos;
	float3 cameraDir;
	float2 cameraFNPlane;
	float4 vWBasisX;
	float4 vWBasisY;
	float4 vWBasisZ;
	float2 UVToViewA;
	float2 UVToViewB;
};

cbuffer Time : register(b2)//VS, DS, PS
{
	double time;
	matrix EVRotation;
	uint lightCount;
	float4 screenInfo;
	bool TAA;
	bool HBAO;
	bool HDR;
	float GIScale;
	bool volumetricRendering;
	bool skyRendering;
};

cbuffer DirectionalLightCBuf : register(b3)//PS
{
	float3 direction;
	float3 DdiffuseColor;
	float DdiffuseIntensity;
};

cbuffer PointLightCBuf : register(b4)//PS
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ShadowTransformCBuf : register(b5) //VS, may PS in deferred
{
	matrix shadowMatrix_VP;
};

cbuffer ShadowControl : register(b6)//PS
{
	int pcfLevel;
	float depthBias;
	bool hwPcf;
	float cubeShadowBaseOffset;
}