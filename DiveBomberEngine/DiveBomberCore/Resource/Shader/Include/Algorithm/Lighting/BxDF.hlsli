void PhongShading(inout LightingResult litRes, GBuffer gBuffer, LightData litData, float3 V)
{
	// diffuse light
	litRes.diffuseLighting = Diffuse_New(litData.irradiance, litData.dirToL, gBuffer.normal) * gBuffer.baseColor;
	// specular reflected
	litRes.specularLighting = Speculate_New(normalize(V + litData.dirToL), litData.irradiance * gBuffer.specularColor,
		gBuffer.specularWeight, gBuffer.normal, gBuffer.specularGloss);
}

void PhongAmbientShading(out float3 ambientLighting, GBuffer gBuffer, float3 ambient)
{
	ambientLighting = ambient * gBuffer.baseColor;
}

void PBRShading(inout LightingResult litRes, GBuffer gBuffer, LightData litData, float3 V)
{
	const float NdotV = max(dot(gBuffer.normal, V), 0.0f);
	const float NdotL = max(dot(gBuffer.normal, litData.dirToL), 0.0f);
	const float3 halfDir = normalize(litData.dirToL + V);
	const float NdotH = max(dot(gBuffer.normal, halfDir), 0.0f);

	float3 F0 = float3(0.08f, 0.08f, 0.08f) * gBuffer.specular;
	F0 = lerp(F0, gBuffer.baseColor, gBuffer.metallic);

	const float NDF = DistributionGGX(NdotH, gBuffer.roughness);
	const float G = GeometrySmith(NdotV, NdotL, gBuffer.roughness);
	const float3 F = FresnelSchlick(max(dot(halfDir, V), 0.0f), F0);

	const float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - gBuffer.metallic;

	const float3 numerator = NDF * G * F;
	const float denominator = 4.0f * NdotV * NdotL + 0.001f;

	litRes.diffuseLighting = kD * gBuffer.baseColor / PI * litData.irradiance * NdotL;

	litRes.specularLighting = numerator / denominator * litData.irradiance * NdotL;
}

void PBRAmbientShading(out float3 ambientLighting, GBuffer gBuffer, float3 V)
{
	const float NdotV = max(dot(gBuffer.normal, V), 0.0f);
	float3 F0 = float3(0.08f, 0.08f, 0.08f) * gBuffer.specular;
	F0 = lerp(F0, gBuffer.baseColor, gBuffer.metallic);
	const float3 rotatedNormal = normalize(mul(gBuffer.normal, (float3x3)EVRotation));
	const float3 R = reflect(normalize(mul(-V, (float3x3)EVRotation)), rotatedNormal);

	const float3 iKS = FresnelSchlickRoughness(NdotV, F0, gBuffer.roughness);
	float3 iKD = 1.0 - iKS;
	iKD *= 1.0 - gBuffer.metallic;
	float3 irradiance = DecodeGamma(SkyMap.Sample(splr, rotatedNormal).rgb);
	float3 iDiffuse = irradiance * gBuffer.baseColor;

	const float MAX_REF_LOD = 4.0f;
	const float3 prefilteredColor = DecodeGamma(SkyMapMip.SampleLevel(splr, R, gBuffer.roughness * MAX_REF_LOD).rgb);
	const float2 brdf = BRDFLUT.Sample(splrClamp, float2(NdotV, gBuffer.roughness)).rg;
	const float3 iSpecular = prefilteredColor * (iKS * brdf.x + brdf.y);

	ambientLighting = (iKD * iDiffuse + iSpecular) * gBuffer.AO;
}

void LiquidShading(inout LightingResult litRes, GBuffer gBuffer, LightData litData, float3 V)
{
	const float NdotV = max(dot(gBuffer.normal, V), 0.0f);
	const float NdotL = max(dot(gBuffer.normal, litData.dirToL), 0.0f);
	const float3 halfDir = normalize(litData.dirToL + V);
	const float NdotH = max(dot(gBuffer.normal, halfDir), 0.0f);

	gBuffer.baseColor += gBuffer.CustomData0 * litData.irradiance;

	float3 F0 = float3(0.08f, 0.08f, 0.08f) * gBuffer.specular;
	F0 = lerp(F0, gBuffer.baseColor, gBuffer.metallic);

	const float NDF = DistributionGGX(NdotH, gBuffer.roughness);
	const float G = GeometrySmith(NdotV, NdotL, gBuffer.roughness);
	const float3 F = FresnelSchlick(max(dot(halfDir, V), 0.0f), F0);

	const float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - gBuffer.metallic;

	const float3 numerator = NDF * G * F;
	const float denominator = 4.0f * NdotV * NdotL + 0.001f;

	litRes.diffuseLighting = kD * gBuffer.baseColor / PI * litData.irradiance * NdotL;

	litRes.specularLighting = numerator / denominator * litData.irradiance * NdotL;
}

void ToonShading(inout LightingResult litRes, GBuffer gBuffer, LightData litData, float3 V)
{

}

