struct PSIn
{
	float2 uv : Texcoord;
};

SamplerState samp : register(s0);

float4 main(PSIn In) : SV_Target
{
	Texture2D<float4> baseColorMap = ResourceDescriptorHeap[NonUniformResourceIndex(0u)];
	return baseColorMap.Sample(samp, In.uv);
}