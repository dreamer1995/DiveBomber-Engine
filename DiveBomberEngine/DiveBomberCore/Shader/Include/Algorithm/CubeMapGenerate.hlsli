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

static const float PI = 3.14159265359;
float3 ConvolutionCubeMapIrradiance(TextureCube texture, SamplerState samp, float3 front, uint sampleLevel, bool isSRGB)
{
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);
	
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, front);
	up = cross(front, right);
	
	float sampleDelta = 0.025f;
	float nrSamples = 0.0f;
	
	for (float phi = 0.0f; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5 * PI; theta += sampleDelta)
		{
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = (tangentSample.x * right) + (tangentSample.y * up) + (tangentSample.z * front);

			irradiance += SampleCubeTextureLevel(texture, samp, sampleVec, sampleLevel, isSRGB).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	
	return PI * irradiance * (1 / nrSamples);
}