cbuffer PointLightCBuf2 : register(b7)//PS
{
	float3 lightPos2;
	float3 ambient2;
	float3 diffuseColor2;
	float diffuseIntensity2;
	float attConst2;
	float attLin2;
	float attQuad2;
};

cbuffer PointLightCBuf : register(b8)//PS
{
	float3 lightPos3;
	float3 ambient3;
	float3 diffuseColor3;
	float diffuseIntensity3;
	float attConst3;
	float attLin3;
	float attQuad3;
};

struct LightVectorData
{
	float3 irradiance;
	float3 vToL;
	float3 dirToL;
	float distToL;
};