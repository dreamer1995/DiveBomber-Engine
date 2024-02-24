#ifndef __AlgorithmFluidWave__
#define __AlgorithmFluidWave__

float Motion_4WayChaos(Texture2D textureIn, float2 uv, float speed)
{
	float2 offset[4] = { float2(0.000000f,0.000000f),
						 float2(0.418100f,0.354800f),
						 float2(0.864861f,0.148384f),
						 float2(0.651340f,0.751638f) };
	float2 pannerDir[4] = { float2(0.1f,0.1f),
							float2(-0.1f,-0.1f),
							float2(-0.1f,0.1f),
							float2(0.1f,-0.1f) };
	float outPut = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		float textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * time * speed).r;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}

float3 Motion_4WayChaos_Normal(Texture2D textureIn, float2 uv, float speed)
{
	float2 offset[4] = { float2(0.000000f,0.000000f),
						 float2(0.418100f,0.354800f),
						 float2(0.864861f,0.148384f),
						 float2(0.651340f,0.751638f) };
	float2 pannerDir[4] = { float2(0.1f,0.1f),
							float2(-0.1f,-0.1f),
							float2(-0.1f,0.1f),
							float2(0.1f,-0.1f) };
	float3 outPut = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		float3 textureSample = textureIn.Sample(splr, uv + offset[i] + pannerDir[i] * time * speed).rgb;
		outPut += textureSample;
	}

	return outPut * 0.25f;
}

float2 UVRefractionDistorted(float3 Rv, float2 uv, float depth)
{
	return float2(uv.x + Rv.x * depth, uv.y + Rv.z * depth);
}

float4 CalculatePhase(float3 worldPos)
{
	float4 psi = S * w;
	return w * Dx * worldPos.x + w * Dz * worldPos.z + psi * time;
}

float3 CalculateWavesDisplacement(float3 worldPos, float4 sinp, float4 cosp)
{
	float3 Gpos;
	Gpos.x = worldPos.x + dot(Q * A * Dx, cosp);
	Gpos.z = worldPos.z + dot(Q * A * Dz, cosp);
	Gpos.y = dot(A, sinp);
	return Gpos;
}

float CalculateWavesDisplacement(float4 sinp)
{
	return dot(A, sinp);
}

float3 CalculateTangent(float4 sinp, float4 cosp)
{
	float3 Gtan;
	Gtan.x = 1.0f - dot(Q * A * w * Dx * Dx, sinp);
	Gtan.y = dot(A * w * Dx, cosp);
	Gtan.z = -dot(Q * A * w * Dz * Dx, sinp);
	return Gtan;
}

float3 CalculateBinormal(float4 sinp, float4 cosp)
{
	float3 GBin;
	GBin.x = -dot(Q * A * w * Dz * Dx, sinp);
	GBin.y = dot(A * w * Dz, cosp);
	GBin.z = 1.0f - dot(Q * A * w * Dz * Dz, sinp);
	return GBin;
}

float GenerateDepth(float worldPosZ)
{
	float linearPosZ = worldPosZ * 0.1f + 0.5f;
	return linearPosZ * linearPosZ;
}

#endif // __AlgorithmFluidWave__