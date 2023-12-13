//Used 0,1,2

cbuffer ShadowTransformCBuf0 : register(b7)//VS
{
	matrix shadowMatrix_M0;
};

cbuffer ShadowTransformCBuf1 : register(b8)//VS
{
	matrix shadowMatrix_M1;
};

cbuffer ShadowTransformCBuf2 : register(b9)//VS
{
	matrix shadowMatrix_M2;
};
