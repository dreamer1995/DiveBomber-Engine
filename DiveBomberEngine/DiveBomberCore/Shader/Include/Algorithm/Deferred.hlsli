float EncodeShadingModelID(uint ShadingModelID)
{
	return (float)ShadingModelID / (float)0xFF;
}
uint DecodeShadingModelID(float channelVal)
{
	return (uint)round(channelVal * (float)0xFF);
}

//#define EncodeNormal(a) (a*127.0/255.0 + 127.0/255.0)
//#define DecodeNormal(a) (a*255.0/127.0 - 1.0)
float3 EncodeNormal( float3 N )
{
	return N * 0.5f + 0.5f;
	//return Pack1212To888( UnitVectorToOctahedron( N ) * 0.5 + 0.5 );
}
float3 DecodeNormal( float3 N )
{
	return N * 2 - 1;
	//return OctahedronToUnitVector( Pack888To1212( N ) * 2 - 1 );
}

float ConvertFromLinearDepth(float linearDepth)
{
	float ProjectionA = cameraFNPlane.x / (cameraFNPlane.x - cameraFNPlane.y);
	float ProjectionB = (-cameraFNPlane.x * cameraFNPlane.y) / (cameraFNPlane.x - cameraFNPlane.y);

	// Sample the depth and convert to linear view space Z (assume it gets sampled as
	// a floating point value of the range [0,1])
	float depth = ProjectionB / linearDepth * cameraFNPlane.x + ProjectionA;
	return depth;
	//return (CameraInfo.x / linearDepth - CameraInfo.x) / (CameraInfo.y - CameraInfo.x);
}

float ConvertToLinearDepth(float depth)
{
	// Calculate our projection constants (you should of course do this in the app code, I'm just showing how to do it)
	float ProjectionA = cameraFNPlane.x / (cameraFNPlane.x - cameraFNPlane.y);
	float ProjectionB = (-cameraFNPlane.x * cameraFNPlane.y) / (cameraFNPlane.x - cameraFNPlane.y);

	// Sample the depth and convert to linear view space Z (assume it gets sampled as
	// a floating point value of the range [0,1])
	float linearDepth = ProjectionB / (depth - ProjectionA) / cameraFNPlane.x;
	return linearDepth;
	//return CameraInfo.x / (depth * (CameraInfo.y - CameraInfo.x) + CameraInfo.x);
}

float3 CalcHomogeneousPos(float depth, float2 screenTC)
{
	float3 pos = vWBasisZ.xyz + (vWBasisX.xyz * screenTC.x) + (vWBasisY.xyz * screenTC.y);
	return pos * depth;
}

float2 ScreenSpace2NDC(float2 sc)
{
	return float2(sc.x * 2.0f - 1.0f, 1.0f - sc.y * 2.0f);
}

float2 NDC2ScreenSpace(float2 ndc)
{
	return float2(0.5f + 0.5f * ndc.x, 0.5f - 0.5f * ndc.y);
}

float Luminance(float3 LinearColor)
{
	return dot(LinearColor, float3(0.3f, 0.59f, 0.11f));
}