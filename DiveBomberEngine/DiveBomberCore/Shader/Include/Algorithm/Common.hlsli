#define EncodeGammaWithAtten(x) pow(x / (x + 1.0f), 1.0f / 2.2f)

//https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
float3 DecodeGamma(in float3 sRGBCol)
{
	float3 linearRGBLo = sRGBCol / 12.92;
	float3 linearRGBHi = pow((sRGBCol + 0.055) / 1.055, 2.4);
	float3 linearRGB = (sRGBCol <= 0.04045) ? linearRGBLo : linearRGBHi;
	return linearRGB;
}

float3 EncodeGamma(in float3 linearCol)
{
	float3 sRGBLo = linearCol * 12.92;
	float3 sRGBHi = (pow(abs(linearCol), 1.0 / 2.4) * 1.055) - 0.055;
	float3 sRGB = (linearCol <= 0.0031308) ? sRGBLo : sRGBHi;
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