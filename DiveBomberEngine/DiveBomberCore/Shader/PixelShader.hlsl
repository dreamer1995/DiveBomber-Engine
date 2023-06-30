struct PixelShaderInput
{
	float4 Color    : COLOR;
};

float4 main(PixelShaderInput IN, uint id : SV_PrimitiveID) : SV_Target
{
	return IN.Color;
}