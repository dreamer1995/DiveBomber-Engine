#ifndef __AlgorithmLighting__
#define __AlgorithmLighting__

#define ShadingModel_UnLit 0u
#define ShadingModel_Phong 1u
#define ShadingModel_PBR 2u
#define ShadingModel_Liquid 3u
#define ShadingModel_Toon 4u

float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

float3 Diffuse(
    uniform float3 irradiance,
    const in float att,
    const in float3 lightDir,
    const in float3 normal)
{
    return irradiance * att * max(0.0f, dot(normal, lightDir));
}

float3 Speculate(
    const in float3 viewPos,
    const in float3 fragPos,
    const in float3 lightDir,
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 normal,
    const in float att,
    const in float specularPower)
{
    // vector from camera to fragment (in view space)
    const float3 viewDir = normalize(viewPos - fragPos);
    // calculate half light vector
    const float3 halfDir = normalize(lightDir + viewDir);
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(normal, halfDir)), specularPower);
}

float3 Diffuse_New(
    uniform float3 irradiance,
    const in float3 lightDir,
    const in float3 normal)
{
    return irradiance * max(0.0f, dot(normal, lightDir));
}

float3 Speculate_New(
    const float3 halfDir,
    const in float3 irradiance,
    uniform float specularIntensity,
    const in float3 normal,
    const in float specularPower)
{
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return irradiance * specularIntensity * pow(max(0.0f, dot(normal, halfDir)), specularPower);
}

float4 ToShadowHomoSpace(const in float4 worldPos, const matrix shadowMatrix)
{
    const float4 shadowHomo = mul(worldPos, shadowMatrix);
    return shadowHomo * float4(0.5f, -0.5f, 1.0f, 1.0f) + float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w;
}

float ShadowLoop_(const in float3 spos, uniform int range, const Texture2D shadowMap)
{    
    float shadowLevel = 0.0f;
    [unroll]
    for (int x = -range; x <= range; x++)
    {
        [unroll]
        for (int y = -range; y <= range; y++)
        {
            if( hwPcf )
            {
                shadowLevel += shadowMap.SampleCmpLevelZero(ssamHw, spos.xy, spos.b - depthBias, int2(x, y));
            }
            else
            {
                shadowLevel += shadowMap.Sample(ssamSw, spos.xy, int2(x, y)).r >= spos.b - depthBias ? 1.0f : 0.0f;
            }
        }
    }
    return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));
}

float Shadow(const in float4 shadowHomoPos, const Texture2D shadowMap)
{    
    float shadowLevel = 0.0f;
    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    
    if( spos.z > 1.0f || spos.z < 0.0f )
    {
        shadowLevel = 1.0f;
    }
    else
    {
        [unroll]
        for (int level = 0; level <= 4; level++)
        {
            if (level == pcfLevel)
            {
                shadowLevel = ShadowLoop_(spos, level, shadowMap);
            }
        }
    }
    return shadowLevel;
}

float4 ToCubeShadowWorldSpace(const in float4 worldPos, uniform matrix shadowMatrix)
{
    return mul(worldPos, shadowMatrix);
}

float CubeShadowLoop_(const in float3 shadowPos, const in float spos, const unsigned int2 basisIndex,
    uniform int range, const TextureCube shadowMap)
{
    float shadowLevel = 0.0f;
    [unroll]
    for (int x = -range; x <= range; x++)
    {
        [unroll]
        for (int y = -range; y <= range; y++)
        {
            float3 shadowPosBias;
            if (basisIndex.x == 1)
                shadowPosBias = float3(x * cubeShadowBaseOffset, 0.0f, 0.0f);
            else
                shadowPosBias = float3(0.0f, x * cubeShadowBaseOffset, 0.0f);
            if (basisIndex.y == 2)
                shadowPosBias += float3(0.0f, y * cubeShadowBaseOffset, 0.0f);
            else
                shadowPosBias += float3(0.0f, 0.0f, y * cubeShadowBaseOffset);

            shadowPosBias += shadowPos;

            if (hwPcf)
            {
                shadowLevel += shadowMap.SampleCmpLevelZero(ssamHw, shadowPosBias, spos);
            }
            else
            {
                shadowLevel += shadowMap.Sample(ssamSw, shadowPosBias).r >= spos ? 1.0f : 0.0f;
            }
        }
    }
    return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));
}

void CalculateShadowDepth(const float4 shadowPos, out unsigned int2 basisIndex, out float spos)
{
    // get magnitudes for each basis component
    const float3 m = abs(shadowPos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)

    float major = max(m.x, max(m.y, m.z));

    if (m.x > m.y)
    {
        major = m.x;
        basisIndex = (unsigned int2)(2, 4);

    }
    else
    {
        major = m.y;
        basisIndex = (unsigned int2)(1, 4);
    }
    if (major < m.z)
    {
        major = m.z;
        basisIndex = (unsigned int2)(1, 2);
    }

    // converting from distance in shadow light space to projected depth
    spos = (c1 * major + c0) / major;
}

float CubeShadow(const in float4 shadowPos, const TextureCube shadowMap)
{
    float shadowLevel = 0.0f;
    unsigned int2 basisIndex = (unsigned int2)(0, 0);
    float spos = 0;
    CalculateShadowDepth(shadowPos, basisIndex, spos);

    if (spos > 1.0f || spos < 0.0f)
    {
        shadowLevel = 1.0f;
    }
    else
    {
        [unroll]
        for (int level = 0; level <= 4; level++)
        {
            if (level == pcfLevel)
            {
                shadowLevel = CubeShadowLoop_(shadowPos.xyz, spos, basisIndex, level, shadowMap);
            }
        }
    }
    return shadowLevel;
}

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
	LightVectorData lv;
	lv.irradiance = diffuseColor * diffuseIntensity;
	lv.vToL = lightPos - fragPos;
	lv.distToL = length(lv.vToL);
	lv.dirToL = lv.vToL / lv.distToL;
	return lv;
}

#endif // __AlgorithmLighting__