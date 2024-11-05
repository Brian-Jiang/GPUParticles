
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Sibling/Children includes
#include "SetPerMaterialConstantsShaderAction.h"
namespace PE {
using namespace Components;

void SetPerMaterialConstantsShaderAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#if PE_PLAT_IS_PSVITA
#	elif APIABSTRACTION_D3D11

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	Effect::setConstantBuffer(pDevice, pDeviceContext, s_pBuffer, 3, &m_data, sizeof(Data));
#	elif APIABSTRACTION_D3D9
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
	pDevice->SetVertexShaderConstantF(159, (const float *)(&m_data), sizeof(m_data) / (4*sizeof(float)));
	pDevice->SetPixelShaderConstantF(159, (const float *)(&m_data), sizeof(m_data) / (4*sizeof(float)));
#	elif APIABSTRACTION_OGL
	ExternalPerTechniqueData &data = pCurEffect->m_externalPerTechniqueData;
		
#if APIABSTRACTION_IOS
    
    glUniform4fv(data.mcbPerMaterial_c3_cgparameters.v_xHasNrm_yHasSpec_zHasGlow_wHasDiff, 1,
                        (const GLfloat*)(&m_data.m_xHasNrm_yHasSpec_zHasGlow_wHasDiff));
	IRenderer::checkForErrors("setting parameter values\n");
    
	//glUniform4fv(data.mcbPerMaterial_c3_cgparameters.f_xHasNrm_yHasSpec_zHasGlow_wHasDiff, 1,
    //                    (const GLfloat*)(&m_data.m_xHasNrm_yHasSpec_zHasGlow_wHasDiff));
	//IRenderer::checkForErrors("setting parameter values\n");
    
	glUniform4fv(data.mcbPerMaterial_c3_cgparameters.v_gVDiffuse, 1,
                        (const GLfloat*)(&m_data.m_diffuse));
	IRenderer::checkForErrors("setting parameter values\n");
    
	//glUniform4fv(data.mcbPerMaterial_c3_cgparameters.f_gVDiffuse, 1,
    //                    (const GLfloat*)(&m_data.m_diffuse));
	//IRenderer::checkForErrors("setting parameter values\n");
    
	glUniform4fv(data.mcbPerMaterial_c3_cgparameters.v_gxyzVSpecular_w, 1,
                        (const GLfloat*)(&m_data.gxyzVSpecular_w));
	IRenderer::checkForErrors("setting parameter values\n");
    
	//glUniform4fv(data.mcbPerMaterial_c3_cgparameters.f_gxyzVSpecular_w, 1,
    //                    (const GLfloat*)(&m_data.gxyzVSpecular_w));
	//IRenderer::checkForErrors("setting parameter values\n");
    
	glUniform4fv(data.mcbPerMaterial_c3_cgparameters.v_gxyzVEmissive_wVShininess, 1,
                        (const GLfloat*)(&m_data.gxyzVEmissive_wVShininess));
	IRenderer::checkForErrors("setting parameter values\n");
    
	//glUniform4fv(data.mcbPerMaterial_c3_cgparameters.f_gxyzVEmissive_wVShininess, 1,
    //                    (const GLfloat*)(&m_data.gxyzVEmissive_wVShininess));
	//IRenderer::checkForErrors("setting parameter values\n");
    
#else
    
	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.v_xHasNrm_yHasSpec_zHasGlow_wHasDiff,
		(const GLfloat*)(&m_data.m_xHasNrm_yHasSpec_zHasGlow_wHasDiff));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.f_xHasNrm_yHasSpec_zHasGlow_wHasDiff,
		(const GLfloat*)(&m_data.m_xHasNrm_yHasSpec_zHasGlow_wHasDiff));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.v_gVDiffuse,
		(const GLfloat*)(&m_data.m_diffuse));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.f_gVDiffuse,
		(const GLfloat*)(&m_data.m_diffuse));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.v_gxyzVSpecular_w,
		(const GLfloat*)(&m_data.gxyzVSpecular_w));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.f_gxyzVSpecular_w,
		(const GLfloat*)(&m_data.gxyzVSpecular_w));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.v_gxyzVEmissive_wVShininess,
		(const GLfloat*)(&m_data.gxyzVEmissive_wVShininess));
	IRenderer::checkForErrors("setting parameter values\n");

	cgGLSetParameter4fv(data.mcbPerMaterial_c3_cgparameters.f_gxyzVEmissive_wVShininess,
		(const GLfloat*)(&m_data.gxyzVEmissive_wVShininess));
	IRenderer::checkForErrors("setting parameter values\n");
#endif
	//cgGLUpdateProgramParameters(pCurEffect->m_cgVertexProgram);
	//cgGLUpdateProgramParameters(pCurEffect->m_cgFragmentProgram);

#	endif
}

void SetPerMaterialConstantsShaderAction::unbindFromPipeline(Components::Effect *pCurEffect/* = NULL*/)
{}

void SetPerMaterialConstantsShaderAction::releaseData()
{}


#if APIABSTRACTION_OGL
void SetPerMaterialConstantsShaderAction::PerEffectBindIds::initialize(Effect *pEffect)
{
#if APIABSTRACTION_IOS 
    
	v_xHasNrm_yHasSpec_zHasGlow_wHasDiff = glGetUniformLocation(pEffect->m_glslProgram, "xHasNrm_yHasSpec_zHasGlow_wHasDiff");
	NSLog(@"xHasNrm_yHasSpec_zHasGlow_wHasDiff uniform location: %d", v_xHasNrm_yHasSpec_zHasGlow_wHasDiff);
    
	//f_xHasNrm_yHasSpec_zHasGlow_wHasDiff = glGetUniformLocation(pEffect->m_glslProgram, "xHasNrm_yHasSpec_zHasGlow_wHasDiff");
	//NSLog(@"xHasNrm_yHasSpec_zHasGlow_wHasDiff uniform location: %d", f_xHasNrm_yHasSpec_zHasGlow_wHasDiff);
    
	v_gVDiffuse = glGetUniformLocation(pEffect->m_glslProgram, "gVDiffuse");
	NSLog(@"gVDiffuse uniform location: %d", v_gVDiffuse);
    
	//f_gVDiffuse = glGetUniformLocation(pEffect->m_glslProgram, "gVDiffuse");
	//NSLog(@"gVDiffuse uniform location: %d", f_gVDiffuse);
    
	v_gxyzVSpecular_w = glGetUniformLocation(pEffect->m_glslProgram, "gxyzVSpecular_w");
	NSLog(@"gxyzVSpecular_w uniform location: %d", v_gxyzVSpecular_w);
    
	//f_gxyzVSpecular_w = glGetUniformLocation(pEffect->m_glslProgram, "gxyzVSpecular_w");
	//NSLog(@"gxyzVSpecular_w uniform location: %d", f_gxyzVSpecular_w);
    
	v_gxyzVEmissive_wVShininess = glGetUniformLocation(pEffect->m_glslProgram, "gxyzVEmissive_wVShininess");
	NSLog(@"gxyzVEmissive_wVShininess uniform location: %d", v_gxyzVEmissive_wVShininess);
    
	//f_gxyzVEmissive_wVShininess = glGetUniformLocation(pEffect->m_glslProgram, "gxyzVEmissive_wVShininess");
	//NSLog(@"gxyzVEmissive_wVShininess uniform location: %d", f_gxyzVEmissive_wVShininess);
    
#else
	v_xHasNrm_yHasSpec_zHasGlow_wHasDiff = cgGetNamedParameter(pEffect->m_cgVertexProgram, "xHasNrm_yHasSpec_zHasGlow_wHasDiff");
	IRenderer::checkForErrors("could not get xHasNrm_yHasSpec_zHasGlow_wHasDiff parameter\n");

	f_xHasNrm_yHasSpec_zHasGlow_wHasDiff = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "xHasNrm_yHasSpec_zHasGlow_wHasDiff");
	IRenderer::checkForErrors("could not get xHasNrm_yHasSpec_zHasGlow_wHasDiff parameter\n");

	v_gVDiffuse = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gVDiffuse");
	IRenderer::checkForErrors("could not get gVDiffuse parameter\n");

	f_gVDiffuse = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gVDiffuse");
	IRenderer::checkForErrors("could not get gVDiffuse parameter\n");

	v_gxyzVSpecular_w = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gxyzVSpecular_w");
	IRenderer::checkForErrors("could not get gxyzVSpecular_w parameter\n");

	f_gxyzVSpecular_w = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gxyzVSpecular_w");
	IRenderer::checkForErrors("could not get gxyzVSpecular_w parameter\n");

	v_gxyzVEmissive_wVShininess = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gxyzVEmissive_wVShininess");
	IRenderer::checkForErrors("could not get gxyzVEmissive_wVShininess parameter\n");

	f_gxyzVEmissive_wVShininess = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gxyzVEmissive_wVShininess");
	IRenderer::checkForErrors("could not get gxyzVEmissive_wVShininess parameter\n");
#endif
}
#endif


}; // namespace PE
