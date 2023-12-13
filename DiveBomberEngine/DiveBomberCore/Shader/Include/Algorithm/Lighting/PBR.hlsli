TextureCube SkyMap : register(t11);
TextureCube SkyMapMip : register(t12);
Texture2D BRDFLUT : register(t13);

SamplerState splr;
SamplerState splrClamp : register(s1);

static const float PI = 3.14159265359f;

float DistributionGGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float _denominator = NdotH2 * (a2 - 1.0f) + 1.0f;
	float denominator = PI * _denominator * _denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float dotedVector, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;
	float numerator = dotedVector;
	float denominator = dotedVector * (1.0f - k) + k;

	return numerator / denominator;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float HdotV, float3 F0)
{
	return F0 + (1.0f - F0)*pow(1.0 - HdotV, 5.0f);
}

float3 FresnelSchlickRoughness(float NdotV, float3 F0, float roughness)
{
	float _roughness = 1.0f - roughness;
	return F0 + (max(float3(_roughness, _roughness, _roughness), F0) - F0) * pow(1.0 - NdotV, 5.0f);
}

float IBLGeometrySchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
	float r = roughness;
	float k = (r * r) / 2.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

float IBLGeometrySmith(float3 normalVec, float3 viewDir, float3 lightDir, float k)
{
	float NdotV = max(dot(normalVec, viewDir), 0.0f);
	float NdotL = max(dot(normalVec, lightDir), 0.0f);
	float ggx1 = IBLGeometrySchlickGGX(NdotV, k);
	float ggx2 = IBLGeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

// VanDerCorpus calculation
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 normalVec)
{
	float a = Roughness * Roughness;
	float Phi = 2.0f * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);
	// from spherical coordinates to cartesian coordinates - halfway vector
	float3 halfwayVec;
	halfwayVec.x = SinTheta * cos(Phi);
	halfwayVec.y = SinTheta * sin(Phi);
	halfwayVec.z = CosTheta;
	// from tangent-space H vector to world-space sample vector
	float3 UpVector = abs(normalVec.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, normalVec));
	float3 TangentY = cross(normalVec, TangentX);
	// Tangent to world space
	return normalize((TangentX * halfwayVec.x) + (TangentY * halfwayVec.y) + (normalVec * halfwayVec.z));
}
