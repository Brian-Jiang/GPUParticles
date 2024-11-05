#ifndef HLSL_LIGHTHELPER
#define HLSL_LIGHTHELPER

// Light and material structs
struct SurfaceInfo
{
	float3 pos;
	float3 normal;
	float4 diffuse;
	float4 spec;
};

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

// calculation functions for each type of light
float3 ParallelLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = make_float3(0, 0, 0);
	
	//Light vector (opposite photon direction) //?: + and now works
	float3 lightVec = -L.xyzDir_w.xyz;
	
	//Add the ambient term.
	litColor += v.diffuse.xyz * L.ambient.xyz;
	
	//Add diffuse and spec term, if surface has los to the light
	float diffuseFactor = max(dot(lightVec, v.normal), 0.0);
	
	float specPower = max(v.spec.a, 1.0);
	float3 toEye	= -normalize(eyePos - v.pos);//?: + and now works
	float3 R		= reflect(lightVec, v.normal);
	float specFactor = pow(max(dot(R, toEye), 0.0), specPower);
		
	//diffuse and sepcular terms
	litColor += diffuseFactor * v.diffuse.xyz * L.diffuse.xyz;
	litColor += specFactor * v.spec.xyz * L.spec.xyz;
	
	return litColor;
}

float3 PointLight(SurfaceInfo v, Light l, float3 eyePos, float shadowFactor)
{
	float3 litColor = make_float3(0, 0, 0);
	
	// Surface to light vector
	float3 lightVec =  l.xyzPos_w.xyz - v.pos;
	
	// Distance from surface to light
	float d = length(lightVec);
	
	
	if( d > l.xRange_yType_zw.x )
		return litColor;
		
	// Normalize the light vector
	lightVec /= d;
	
	
	
	float diffuseFactor = max(dot(lightVec, v.normal), 0.0);
	float specPower		= max(v.spec.a, 1.0);
	float3 toEye		= normalize(eyePos - v.pos);
	float3 R			= reflect( -lightVec, v.normal);
	float specFactor	= pow(max(dot(R, toEye), 0.0), specPower);
	
	litColor += shadowFactor * diffuseFactor * v.diffuse.xyz * l.diffuse.xyz;
	litColor += shadowFactor * specFactor * v.spec.xyz * l.spec.xyz;

	
	// attenuate
	litColor /= dot(l.xyzAtt_wSpotPwr.xyz, make_float3(1.0, d, d*d));
	
	// Add ambient light
	litColor += v.diffuse.xyz * l.ambient.xyz;
	
	return litColor;
	
}

float3 SpotLight(SurfaceInfo v, Light L, float3 eyePos, float shadowFactor)
{
	float3 litColor = PointLight(v, L, eyePos, shadowFactor);
	
	//The vector from the surface to the light
	float3 lightVec = normalize(L.xyzPos_w.xyz- v.pos);
	
	//Scale color by spotlight factor
	float s = pow(max(dot(-lightVec, L.xyzDir_w.xyz), 0.0), L.xyzAtt_wSpotPwr.w);
	
	return litColor * s;
}

float3 RenderLight(SurfaceInfo v, Light l, float3 eyePos, float shadowFactor)
{
    if (l.xRange_yType_zw.y < 0.1) // == 0)
		return PointLight(v, l, eyePos, shadowFactor);
	else if (l.xRange_yType_zw.y < 1.1) // == 1)
		return ParallelLight(v, l, eyePos);
	else if (l.xRange_yType_zw.y < 2.1) // == 2)
		return SpotLight(v, l, eyePos, shadowFactor);
	else
		return PointLight(v, l, eyePos, shadowFactor);
}
#endif
