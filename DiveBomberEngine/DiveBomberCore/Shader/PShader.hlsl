struct PSIn
{
	float2 uv : Texcoord;
};

SamplerState samp : register(s0);

float4 main(PSIn In) : SV_Target
{
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(0u)];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(1u)];
	
	float4 baseColor = baseMap.Sample(samp, In.uv);
	float4 rustColor = rustMap.Sample(samp, In.uv);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g);
	return baseColor;
}