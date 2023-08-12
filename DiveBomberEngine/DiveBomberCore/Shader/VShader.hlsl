struct ModelViewProjection
{
	matrix matrix_MVP;
	matrix matrix_MV;
	matrix matrix_V;
	matrix matrix_P;
	matrix matrix_VP;
	matrix matrix_T_MV;
	matrix matrix_IT_MV;
	matrix matrix_M2W;
	matrix matrix_W2M;
	matrix matrix_I_V;
	matrix matrix_I_P;
	matrix matrix_I_VP;
	matrix matrix_I_MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VSIn
{
    float3 pos : Position;
    float3 n : Normal;
    float3 t : Tangent;
    float3 b : Binormal;
    float2 uv : Texcoord;
};

struct VSOut
{
    float2 uv	: Texcoord;
    float4 hPos : SV_Position;
};

VSOut main(VSIn In)
{
    VSOut Out;

    Out.hPos = mul(float4(In.pos, 1.0f), ModelViewProjectionCB.matrix_MVP);
    Out.uv = In.uv;

    return Out;
}