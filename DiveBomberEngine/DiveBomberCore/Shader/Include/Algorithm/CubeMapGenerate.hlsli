#include "..\Common\PBR.hlsli"

static float2 invAtan = float2(0.1591f, 0.3182f);
float2 SampleSphereicalMap(float3 tc)
{
	float2 position = float2(atan2(tc.z, -tc.x), asin(-tc.y));
	position *= invAtan;
	position += 0.5;
	return position;
}

float3 ThreadIDToCubeFaceCoordinate(float2 position, uint groupIndex)
{
	switch (groupIndex)
	{
		case 0:
			return float3(0.5f, -position.y, -position.x);
		case 1:
			return float3(-0.5f, -position.y, position.x);
		case 2:
			return float3(position.x, 0.5f, position.y);
		case 3:
			return float3(position.x, -0.5f, -position.y);
		case 4:
			return float3(position.x, -position.y, 0.5f);
		case 5:
			return float3(-position.x, -position.y, -0.5f);
	}
	
	return float3(0.0f, 0.0f, 0.0f);
}

float3 ConvolutionCubeMapDiffuse(TextureCube texture, SamplerState samp, float3 front, bool isSRGB)
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, front);
	up = cross(front, right);
	
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);
	
	float sampleDelta = 0.025f;
	float nrSamples = 0.0f;
	
	for (float phi = 0.0f; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5 * PI; theta += sampleDelta)
		{
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = (tangentSample.x * right) + (tangentSample.y * up) + (tangentSample.z * front);

			irradiance += SampleCubeTextureLevel(texture, samp, sampleVec, 0u, isSRGB).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	
	return PI * irradiance * (1 / nrSamples);
}

float3 ConvolutionCubeMapSpecular(TextureCube texture, SamplerState samp, float3 normalVec, float roughness, bool isSRGB)
{
	float3 reflectionDir = normalVec;
	float3 viewDir = reflectionDir;
	
	float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
	float totalWeight = 0.0f;
	
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 halfwayVec = ImportanceSampleGGX(Xi, roughness, normalVec);
		float3 lightDir = 2.0f * dot(viewDir, halfwayVec) * halfwayVec - viewDir;
		float NdotL = saturate(dot(normalVec, lightDir));
		if (NdotL > 0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float NdotH = max(dot(normalVec, halfwayVec), 0.0f);
			float D = DistributionGGX(NdotH, roughness);
			float HdotV = max(dot(halfwayVec, viewDir), 0.0f);
			float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

			float resolution = 512.0f; // resolution of source cubemap (per face)
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(NumSamples) * pdf + 0.0001f);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			PrefilteredColor += SampleCubeTextureLevel(texture, samp, lightDir, mipLevel, isSRGB).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	
	return PrefilteredColor /= totalWeight;
}