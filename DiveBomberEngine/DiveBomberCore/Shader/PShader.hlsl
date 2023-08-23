struct PSIn
{
	float2 uv : Texcoord;
};

struct IndexConstant
{
	uint transformIndex[1];
	uint texureIndex[2];
};

ConstantBuffer<IndexConstant> IndexConstantCB : register(b0);

SamplerState samp : register(s0);

float4 main(PSIn In) : SV_Target
{
	Texture2D<float4> baseMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texureIndex[0])];
	Texture2D<float4> rustMap = ResourceDescriptorHeap[NonUniformResourceIndex(IndexConstantCB.texureIndex[1])];
	
	float4 baseColor = baseMap.Sample(samp, In.uv);
	float4 rustColor = rustMap.Sample(samp, In.uv);
	
	float4 color = lerp(baseColor, rustColor, rustColor.g);
	return baseColor;
}