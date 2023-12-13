#include "ConstantsVS.hlsli"

VSOut MainVS(VSIn v)
{
    VSOut o;

#ifndef NoHPos
    o.pos = mul(float4(v.pos, 1.0f), matrix_MVP);
#endif

    float4 worldPos = mul(float4(v.pos, 1.0f), matrix_M2W);
    o.worldPos = (float3)worldPos;

#ifndef NoNormal
    o.normal = normalize(mul(v.n, (float3x3)matrix_M2W));
#endif

#ifndef NoTangent
    o.tangent = normalize(mul(v.t, (float3x3)matrix_M2W));
    o.binormal = normalize(mul(v.b, (float3x3)matrix_M2W));
#endif

#ifndef NoUV
    o.uv = v.uv;
#endif

#ifndef NoShadow
    o.shadowHomoPos = ToShadowHomoSpace(worldPos, shadowMatrix_VP);
    o.shadowCubeWorldPos0 = ToCubeShadowWorldSpace(worldPos, shadowMatrix_M0);
    o.shadowCubeWorldPos1 = ToCubeShadowWorldSpace(worldPos, shadowMatrix_M1);
    o.shadowCubeWorldPos2 = ToCubeShadowWorldSpace(worldPos, shadowMatrix_M2);
#endif

    GetVertexParameters(o, v);
    return o;
}