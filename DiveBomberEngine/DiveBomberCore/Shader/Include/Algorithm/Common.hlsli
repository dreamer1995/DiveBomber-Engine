#define EncodeGammaWithAtten(x) pow(x / (x + 1.0f), 1.0f / 2.2f)

//https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
float3 DecodeGamma(in float3 sRGBCol)
{
	float3 linearRGBLo = sRGBCol / 12.92f;
	float3 linearRGBHi = pow((sRGBCol + 0.055f) / 1.055f, 2.4f);
	float3 linearRGB;
	linearRGB.r = (sRGBCol.r <= 0.04045f) ? linearRGBLo.r : linearRGBHi.r;
	linearRGB.g = (sRGBCol.g <= 0.04045f) ? linearRGBLo.g : linearRGBHi.g;
	linearRGB.b = (sRGBCol.b <= 0.04045f) ? linearRGBLo.b : linearRGBHi.b;
	return linearRGB;
}

float3 EncodeGamma(in float3 linearCol)
{
	float3 sRGBLo = linearCol * 12.92f;
	float3 sRGBHi = (pow(abs(linearCol), 1.0f / 2.4f) * 1.055f) - 0.055f;
	float3 sRGB;
	sRGB.r = (linearCol.r <= 0.0031308f) ? sRGBLo.r : sRGBHi.r;
	sRGB.g = (linearCol.g <= 0.0031308f) ? sRGBLo.g : sRGBHi.g;
	sRGB.b = (linearCol.b <= 0.0031308f) ? sRGBLo.b : sRGBHi.b;
	return sRGB;
}

//Texture2D smap : register(t14);//PS
//TextureCube smap0 : register(t15);//PS
//TextureCube smap1 : register(t16);//PS
//TextureCube smap2 : register(t17);//PS
//SamplerComparisonState ssamHw : register(s2);//PS
//SamplerState ssamSw : register(s3);//PS

// For Cube Shadowing
static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float3 MapNormal(
    const in float3 tan,
    const in float3 binor,
    const in float3 normal,
    const in float2 tc,
    uniform Texture2D nmap,
    uniform SamplerState splr)
{
    // build the tranform (rotation) into same space as tan/binor/normal (target space)
	const float3x3 tanToTarget = float3x3(tan, binor, normal);
    // sample and unpack the normal from texture into target space   
	const float3 normalSample = nmap.Sample(splr, tc).xyz;
	const float3 tanNormal = normalSample * 2.0f - 1.0f;
    // bring normal from tanspace into target space
	return normalize(mul(tanNormal, tanToTarget));
}

float3 RGB2YCoCg(float3 RGB)
{
	float Y = dot(RGB, float3(1, 2, 1));
	float Co = dot(RGB, float3(2, 0, -2));
	float Cg = dot(RGB, float3(-1, 2, -1));

	return float3(Y, Co, Cg);
}

float3 YCoCg2RGB(float3 YCoCg)
{
	float Y = YCoCg.x * 0.25;
	float Co = YCoCg.y * 0.25;
	float Cg = YCoCg.z * 0.25;

	float R = Y + Co - Cg;
	float G = Y + Cg;
	float B = Y - Co - Cg;

	return float3(R, G, B);
}

float Min3(float a, float b, float c)
{
	return min(min(a, b), c);
}

float3 Min3x3(float3 a, float3 b, float3 c)
{
	return float3(Min3(a.x, b.x, c.x), Min3(a.y, b.y, c.y), Min3(a.z, b.z, c.z));
}

float Max3(float a, float b, float c)
{
	return max(max(a, b), c);
}

float3 Max3x3(float3 a, float3 b, float3 c)
{
	return float3(Max3(a.x, b.x, c.x), Max3(a.y, b.y, c.y), Max3(a.z, b.z, c.z));
}

float2 WeightedLerpFactors(float WeightA, float WeightB, float Blend)
{
	float BlendA = (1.0 - Blend) * WeightA;
	float BlendB = Blend * WeightB;
	float RcpBlend = rcp(BlendA + BlendB);
	BlendA *= RcpBlend;
	BlendB *= RcpBlend;
	return float2(BlendA, BlendB);
}

float4 SampleTexture(Texture2D texture, SamplerState samp, float2 uv)
{
	return texture.Sample(samp, uv);
}

float4 SampleTextureLevel(Texture2D texture, SamplerState samp, float2 uv, uint mipLevel)
{
	return texture.SampleLevel(samp, uv, mipLevel);
}

float4 SampleCubeTexture(TextureCube texture, SamplerState samp, float3 uv)
{
	return texture.Sample(samp, uv);
}

float4 SampleCubeTextureLevel(TextureCube texture, SamplerState samp, float3 uv, uint mipLevel)
{
	return texture.SampleLevel(samp, uv, mipLevel);
}

float4 SampleTextureArray(Texture2DArray texture, SamplerState samp, float3 uv)
{
	return texture.Sample(samp, uv);
}

float4 SampleTextureArrayLevel(Texture2DArray texture, SamplerState samp, float3 uv, uint mipLevel)
{
	return texture.SampleLevel(samp, uv, mipLevel);
}

float4 SampleTexture(Texture2D texture, SamplerState samp, float2 uv, bool isSRGB)
{
	float4 outColor = texture.Sample(samp, uv);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}	
	
	return outColor;
}

float4 SampleTextureLevel(Texture2D texture, SamplerState samp, float2 uv, uint mipLevel, bool isSRGB)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}
	
	return outColor;
}

float4 SampleCubeTexture(TextureCube texture, SamplerState samp, float3 uv, bool isSRGB)
{
	float4 outColor = texture.Sample(samp, uv);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}
	
	return outColor;
}

float4 SampleCubeTextureLevel(TextureCube texture, SamplerState samp, float3 uv, uint mipLevel, bool isSRGB)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}
	
	return outColor;
}

float4 SampleTextureArray(Texture2DArray texture, SamplerState samp, float3 uv, bool isSRGB)
{
	float4 outColor = texture.Sample(samp, uv);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}
	
	return outColor;
}

float4 SampleTextureArrayLevel(Texture2DArray texture, SamplerState samp, float3 uv, uint mipLevel, bool isSRGB)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	if (isSRGB)
	{
		outColor.rgb = DecodeGamma(outColor.rgb);
	}
	
	return outColor;
}

float4 SampleSRGBTexture(Texture2D texture, SamplerState samp, float2 uv)
{
	float4 outColor = texture.Sample(samp, uv);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float4 SampleSRGBTextureLevel(Texture2D texture, SamplerState samp, float2 uv, uint mipLevel)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float4 SampleSRGBCubeTexture(TextureCube texture, SamplerState samp, float3 uv)
{
	float4 outColor = texture.Sample(samp, uv);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float4 SampleSRGBCubeTextureLevel(TextureCube texture, SamplerState samp, float3 uv, uint mipLevel)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float4 SampleSRGBTextureArray(Texture2DArray texture, SamplerState samp, float3 uv)
{
	float4 outColor = texture.Sample(samp, uv);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float4 SampleSRGBTextureArrayLevel(Texture2DArray texture, SamplerState samp, float3 uv, uint mipLevel)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	outColor.rgb = DecodeGamma(outColor.rgb);
	
	return outColor;
}

float3 DecodeNormal(float3 normal)
{
	return normal * 2.0f - 1.0f;
}

float3 EncodeNormal(float3 normal)
{
	return normal / 2.0f + 0.5f;
}

float4 SampleNormalTexture(Texture2D texture, SamplerState samp, float2 uv)
{
	float4 outColor = texture.Sample(samp, uv);
	outColor.rbg = DecodeNormal(outColor.rgb);
	
	return outColor;
}

float4 SampleNormalTextureLevel(Texture2D texture, SamplerState samp, float2 uv, uint mipLevel)
{
	float4 outColor = texture.SampleLevel(samp, uv, mipLevel);
	outColor.rbg = DecodeNormal(outColor.rgb);
	
	return outColor;
}