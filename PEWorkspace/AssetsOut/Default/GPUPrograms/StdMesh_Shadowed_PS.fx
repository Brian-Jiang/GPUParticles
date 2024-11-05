#ifdef HLSL_SEPARATE_LOAD

#include "StdMesh.fx"

#include "StandardConstants.fx"
#include "StandardTextureResources.fx"
#include "lighthelper.fx"

#include "SolidBackCullZBias_RS.fx"
#include "ZBufferLookup_DSS.fx"
#include "Replace_BS.fx"

#include "samplerhelper.fx"
#include "shadowmaphelper.fx"
#endif //#ifdef HLSL_SEPARATE_LOAD


float4 StdMesh_Shadowed_PS(STD_MESH_SHADOWED_PS_IN pIn) : SV_Target
{
	normalize(pIn.iNormalW);

	SurfaceInfo defaultSurface = {	pIn.iPosW, //Position
									pIn.iNormalW, //Normal
									make_float4(0.2f, 0.2f, 0.2f, 1.0f), //Diffuse
									make_float4(1.0f, 1.0f, 1.0f, 10.0f), //Specular
								};
	
	
	float shadowFactor = CalcShadowFactor(pIn.iProjTexCoord);
	
	//Combine lighting from all lights
	float4 combinedLighting = {0.0f, 0.0f, 0.0f, 0.0f};
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight0, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight1, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight2, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight3, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight4, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight5, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight6, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
	combinedLighting += make_float4(RenderLight(defaultSurface, gLight7, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);

	float4 dcolor = make_float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 combinedColor = combinedLighting;
	if (xHasNrm_yHasSpec_zHasGlow_wHasDiff.w)
	{
		float4 dcolor = sample(gDiffuseMapSampler, pIn.iTexCoord, gDiffuseMap);
		clip(dcolor.a - 0.75f);
		// factor in color
		combinedColor = combinedLighting * dcolor;
	}

	combinedColor.a = 0.0f;
	
	if (xHasNrm_yHasSpec_zHasGlow_wHasDiff.z)
	{
		// change target alpha based on glow
		float perPixelGlow = sample(gGlowMapSampler, pIn.iTexCoord, gGlowMap).r;
		if (perPixelGlow > 0.1)
		{
			float4 originalColor = sample(gDiffuseMapSampler, pIn.iTexCoord, gDiffuseMap);
			combinedColor = lerp(combinedColor, originalColor, perPixelGlow);
			combinedColor.a = perPixelGlow;
		}
	}
	
	return combinedColor;
}




























