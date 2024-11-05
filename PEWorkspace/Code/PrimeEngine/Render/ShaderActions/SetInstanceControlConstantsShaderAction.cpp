// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "SetInstanceControlConstantsShaderAction.h"

namespace PE {
using namespace Components;
#if PE_API_IS_D3D11
void SetInstanceControlConstantsShaderAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
	#if APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		Effect::setConstantBuffer(pDevice, pDeviceContext, s_pBuffer, 5, &m_data, sizeof(Data));
	#elif APIABSTRACTION_D3D9
    
        int float4PerLight = 7;
        int float4PerLights = float4PerLight * NUM_LIGHT_SOURCES_DATAS; // note shaders are hardcoded to allocate registers in shader files. if these valeus are changed, shaders have to be changed too
    
        int totalFloat4 = (21 + float4PerLights);
        assert(sizeof(m_data) == totalFloat4 * 4 * sizeof(float)); // the data is synced to take totalFloat4 float4 registers on GPU
		IRenderer::Instance()->getDevice()->SetVertexShaderConstantF(1, (const float *)(&m_data), totalFloat4);
		IRenderer::Instance()->getDevice()->SetPixelShaderConstantF(1, (const float *)(&m_data), totalFloat4);
	#elif APIABSTRACTION_OGL
		
		if (pCurEffect)
		{
			ExternalPerTechniqueData &data = pCurEffect->m_externalPerTechniqueData;
		
            bool setVSLights = false;
			#if APIABSTRACTION_IOS 
	        
                // dont bother about setting vs constants since we set them per porgram not vs or ps
                //glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewProj, 1, GL_FALSE,
                //                     (const GLfloat*)(&m_data.gViewProj));
                //IRenderer::checkForErrors("setting parameter values\n");
            
                glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewProj, 1, GL_FALSE,
                                     (const GLfloat*)(&m_data.gViewProj));
                IRenderer::checkForErrors("setting parameter values\n");
            
                //glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gPreviousViewProjMatrix, 1, GL_FALSE,
                //                     (const GLfloat*)(&m_data.gPreviousViewProjMatrix));
                //IRenderer::checkForErrors("setting parameter values\n");
            
                glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gPreviousViewProjMatrix, 1, GL_FALSE,
                                     (const GLfloat*)(&m_data.gPreviousViewProjMatrix));
                IRenderer::checkForErrors("setting parameter values\n");
            
                //glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewProjInverseMatrix, 1, GL_FALSE,
                //                     (const GLfloat*)(&m_data.gViewProjInverseMatrix));
                //IRenderer::checkForErrors("setting parameter values\n");
            
                glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewProjInverseMatrix, 1, GL_FALSE,
                                     (const GLfloat*)(&m_data.gViewProjInverseMatrix));
                IRenderer::checkForErrors("setting parameter values\n");
            
                //if (data.m_cbPerObjectGroup_c1_cgparameters.v_gViewInv)
                //{
                //    glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewInv, 1, GL_FALSE,
                //                         (const GLfloat*)(&m_data.gViewInv));
                //    IRenderer::checkForErrors("setting parameter values\n");
                //}
            
                if (data.m_cbPerObjectGroup_c1_cgparameters.f_gViewInv)
                {
                    glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewInv, 1, GL_FALSE,
                                         (const GLfloat*)(&m_data.gViewInv));
                    IRenderer::checkForErrors("setting parameter values\n");
                }
            
                //glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLightWVP, 1, GL_FALSE,
                //                     (const GLfloat*)(&m_data.gLightWVP));
                //IRenderer::checkForErrors("setting parameter values\n");
            
                glUniformMatrix4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLightWVP, 1, GL_FALSE,
                                     (const GLfloat*)(&m_data.gLightWVP));
                IRenderer::checkForErrors("setting parameter values\n");
            
                //glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_xyzgEyePosW_wDoMotionBlur, 1,
                //               (const GLfloat*)(&m_data.gEyePosW));
                //IRenderer::checkForErrors("setting parameter values\n");
            
                glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_xyzgEyePosW_wDoMotionBlur, 1,
                               (const GLfloat*)(&m_data.gEyePosW));
                IRenderer::checkForErrors("setting parameter values\n");
            
                for (unsigned int i = 0; i < NUM_LIGHT_SOURCES; i++)
                {
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].pos, 1,
                    //                   (const GLfloat*)(&m_data.gLights[i].pos));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].pos, 1,
                                   (const GLfloat*)(&m_data.gLights[i].pos));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].dir, 1,
                    //                   (const GLfloat*)(&m_data.gLights[i].dir));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].dir, 1,
                                   (const GLfloat*)(&m_data.gLights[i].dir));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].ambient, 1,
                    //                    (const GLfloat*)(&m_data.gLights[i].ambient));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].ambient, 1,
                                    (const GLfloat*)(&m_data.gLights[i].ambient));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].diffuse, 1,
                    //                    (const GLfloat*)(&m_data.gLights[i].diffuse));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].diffuse, 1,
                                    (const GLfloat*)(&m_data.gLights[i].diffuse));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].specular, 1,
                    //                    (const GLfloat*)(&m_data.gLights[i].spec));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].specular, 1,
                                    (const GLfloat*)(&m_data.gLights[i].spec));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].atten_spot, 1,
                    //                   (const GLfloat*)(&m_data.gLights[i].att));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].atten_spot, 1,
                                   (const GLfloat*)(&m_data.gLights[i].att));
                
                    //if (setVSLights)
                    //    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].range_type, 1,
                    //                   (const GLfloat*)(&m_data.gLights[i].range));
                
                    glUniform4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].range_type, 1,
                                   (const GLfloat*)(&m_data.gLights[i].range));
                } //fol lights[]
            
			#else // NOT if APIABSTRACTION_IOS
						
				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewProj, 
					(const GLfloat*)(&m_data.gViewProj));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewProj, 
					(const GLfloat*)(&m_data.gViewProj));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.v_gPreviousViewProjMatrix, 
					(const GLfloat*)(&m_data.gPreviousViewProjMatrix));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.f_gPreviousViewProjMatrix, 
					(const GLfloat*)(&m_data.gPreviousViewProjMatrix));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewProjInverseMatrix, 
					(const GLfloat*)(&m_data.gViewProjInverseMatrix));
				IRenderer::checkForErrors("setting parameter values\n");
				
				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewProjInverseMatrix, 
					(const GLfloat*)(&m_data.gViewProjInverseMatrix));
				IRenderer::checkForErrors("setting parameter values\n");

				if (data.m_cbPerObjectGroup_c1_cgparameters.v_gViewInv)
				{
					cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.v_gViewInv, 
						(const GLfloat*)(&m_data.gViewInv));
					IRenderer::checkForErrors("setting parameter values\n");
				}

				if (data.m_cbPerObjectGroup_c1_cgparameters.f_gViewInv)
				{
					cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.f_gViewInv, 
						(const GLfloat*)(&m_data.gViewInv));
					IRenderer::checkForErrors("setting parameter values\n");
				}

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.v_gLightWVP, 
					(const GLfloat*)(&m_data.gLightWVP));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetMatrixParameterfc(data.m_cbPerObjectGroup_c1_cgparameters.f_gLightWVP, 
					(const GLfloat*)(&m_data.gLightWVP));
				IRenderer::checkForErrors("setting parameter values\n");
				
				cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.v_xyzgEyePosW_wDoMotionBlur,
					(const GLfloat)(m_data.gEyePosW.m_x), (const GLfloat)(m_data.gEyePosW.m_y),
					(const GLfloat)(m_data.gEyePosW.m_z), (const GLfloat)(m_data.gDoMotionBlur));
				IRenderer::checkForErrors("setting parameter values\n");

				cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.f_xyzgEyePosW_wDoMotionBlur,
					(const GLfloat)(m_data.gEyePosW.m_x), (const GLfloat)(m_data.gEyePosW.m_y),
					(const GLfloat)(m_data.gEyePosW.m_z), (const GLfloat)(m_data.gDoMotionBlur));
				IRenderer::checkForErrors("setting parameter values\n");
				
				for (unsigned int i = 0; i < NUM_LIGHT_SOURCES; i++)
				{
					if (setVSLights)
					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].pos,
						(const GLfloat)(m_data.gLights[i].pos.m_x), (const GLfloat)(m_data.gLights[i].pos.m_y),
						(const GLfloat)(m_data.gLights[i].pos.m_z), (const GLfloat)(m_data.gLights[i].pad0));

					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].pos,
						(const GLfloat)(m_data.gLights[i].pos.m_x), (const GLfloat)(m_data.gLights[i].pos.m_y),
						(const GLfloat)(m_data.gLights[i].pos.m_z), (const GLfloat)(m_data.gLights[i].pad0));

					if (setVSLights)
					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].dir,
						(const GLfloat)(m_data.gLights[i].dir.m_x), (const GLfloat)(m_data.gLights[i].dir.m_y),
						(const GLfloat)(m_data.gLights[i].dir.m_z), (const GLfloat)(m_data.gLights[i].pad1));

					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].dir,
						(const GLfloat)(m_data.gLights[i].dir.m_x), (const GLfloat)(m_data.gLights[i].dir.m_y),
						(const GLfloat)(m_data.gLights[i].dir.m_z), (const GLfloat)(m_data.gLights[i].pad1));

					if (setVSLights)
					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].ambient,
						(const GLfloat*)(&m_data.gLights[i].ambient));

					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].ambient,
						(const GLfloat*)(&m_data.gLights[i].ambient));

					if (setVSLights)
					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].diffuse,
						(const GLfloat*)(&m_data.gLights[i].diffuse));

					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].diffuse,
						(const GLfloat*)(&m_data.gLights[i].diffuse));

					if (setVSLights)
					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].specular,
						(const GLfloat*)(&m_data.gLights[i].spec));

					cgGLSetParameter4fv(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].specular,
						(const GLfloat*)(&m_data.gLights[i].spec));

					if (setVSLights)
					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].atten_spot,
						(const GLfloat)(m_data.gLights[i].att.m_x), (const GLfloat)(m_data.gLights[i].att.m_y),
						(const GLfloat)(m_data.gLights[i].att.m_z), (const GLfloat)(m_data.gLights[i].spotPower));

					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].atten_spot,
						(const GLfloat)(m_data.gLights[i].att.m_x), (const GLfloat)(m_data.gLights[i].att.m_y),
						(const GLfloat)(m_data.gLights[i].att.m_z), (const GLfloat)(m_data.gLights[i].spotPower));

					if (setVSLights)
					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.v_gLight[i].range_type,
						(const GLfloat)(m_data.gLights[i].range), (const GLfloat)(m_data.gLights[i].type),
						(const GLfloat)(m_data.gLights[i].pad2), (const GLfloat)(m_data.gLights[i].pad3));

					cgGLSetParameter4f(data.m_cbPerObjectGroup_c1_cgparameters.f_gLight[i].range_type,
						(const GLfloat)(m_data.gLights[i].range), (const GLfloat)(m_data.gLights[i].type),
						(const GLfloat)(m_data.gLights[i].pad2), (const GLfloat)(m_data.gLights[i].pad3));
				} //fol lights[]
			
				#if APIABSTRACTION_GLPC
						//cgGLUpdateProgramParameters(data.m_cgVertexProgram);
						//cgGLUpdateProgramParameters(data.m_cgFragmentProgram);
				#endif
			#endif // NOT if APIABSTRACTION_IOS
		} // if (pCurEffect)
	#endif // if APIABSTRACTION_OGL
}

void SetInstanceControlConstantsShaderAction::unbindFromPipeline(Components::Effect *pCurEffect/* = NULL*/)
{}

void SetInstanceControlConstantsShaderAction::releaseData()
{}


#if APIABSTRACTION_OGL
void SetInstanceControlConstantsShaderAction::PerEffectBindIds::initialize(Effect *pEffect)
{
#if APIABSTRACTION_IOS 

	//in ios shader constants are for the whole program, not separated by vertex or pixel
    
    f_gViewProj = v_gViewProj = glGetUniformLocation(pEffect->m_glslProgram, "gViewProj");
	NSLog(@"v_gViewProj uniform location: %d", v_gViewProj);
    IRenderer::checkForErrors("could not get v_gViewProj parameter\n");
    
	//f_gViewProj = glGetUniformLocation(pEffect->m_glslProgram, "gViewProj");
	//NSLog(@"f_gViewProj uniform location: %d", f_gViewProj);
    //IRenderer::checkForErrors("could not get f_gViewProj parameter\n");
    
	f_gPreviousViewProjMatrix = v_gPreviousViewProjMatrix = glGetUniformLocation(pEffect->m_glslProgram, "gPreviousViewProjMatrix");
	NSLog(@"v_gPreviousViewProjMatrix uniform location: %d", v_gPreviousViewProjMatrix);
    IRenderer::checkForErrors("could not get v_gPreviousViewProjMatrix parameter\n");
    
	//f_gPreviousViewProjMatrix = glGetUniformLocation(pEffect->m_glslProgram, "gPreviousViewProjMatrix");
	//NSLog(@"f_gPreviousViewProjMatrix uniform location: %d", f_gPreviousViewProjMatrix);
    //IRenderer::checkForErrors("could not get f_gPreviousViewProjMatrix parameter\n");
    
	f_gViewProjInverseMatrix = v_gViewProjInverseMatrix = glGetUniformLocation(pEffect->m_glslProgram, "gViewProjInverseMatrix");
	NSLog(@"v_gViewProjInverseMatrix uniform location: %d", v_gViewProjInverseMatrix);
    IRenderer::checkForErrors("could not get v_gViewProjInverseMatrix parameter\n");
    
	//f_gViewProjInverseMatrix = glGetUniformLocation(pEffect->m_glslProgram, "gViewProjInverseMatrix");
	//NSLog(@"f_gViewProjInverseMatrix uniform location: %d", f_gViewProjInverseMatrix);
    //IRenderer::checkForErrors("could not get gViewProjInverseMatrix parameter\n");
    
	f_gViewInv = v_gViewInv = glGetUniformLocation(pEffect->m_glslProgram, "gView");
	NSLog(@"v_gViewInv uniform location: %d", v_gViewInv);
    IRenderer::checkForErrors("could not get v_gViewInv parameter\n");
    
	//f_gViewInv = glGetUniformLocation(pEffect->m_glslProgram, "gView");
	//NSLog(@"f_gViewInv uniform location: %d", f_gViewInv);
    //IRenderer::checkForErrors("could not get f_gViewInv parameter\n");
    
	f_gLightWVP = v_gLightWVP = glGetUniformLocation(pEffect->m_glslProgram, "gLightWVP");
	NSLog(@"v_gLightWVP uniform location: %d", v_gLightWVP);
    IRenderer::checkForErrors("could not get v_gLightWVP parameter\n");
    
	//f_gLightWVP = glGetUniformLocation(pEffect->m_glslProgram, "gLightWVP");
	//NSLog(@"f_gLightWVP uniform location: %d", f_gLightWVP);
    //IRenderer::checkForErrors("could not get f_gLightWVP parameter\n");
    
	f_gEyePosW = v_gEyePosW = glGetUniformLocation(pEffect->m_glslProgram, "gEyePosW");
	NSLog(@"v_gEyePosW uniform location: %d", v_gEyePosW);
    IRenderer::checkForErrors("could not get v_gEyePosW parameter\n");
    
	//f_gEyePosW = glGetUniformLocation(pEffect->m_glslProgram, "gEyePosW");
	//NSLog(@"f_gEyePosW uniform location: %d", f_gEyePosW);
    //IRenderer::checkForErrors("could not get f_gEyePosW parameter\n");
    
	f_xyzgEyePosW_wDoMotionBlur = v_xyzgEyePosW_wDoMotionBlur = glGetUniformLocation(pEffect->m_glslProgram, "xyzgEyePosW_wDoMotionBlur");
	NSLog(@"v_xyzgEyePosW_wDoMotionBlur uniform location: %d", v_xyzgEyePosW_wDoMotionBlur);
    IRenderer::checkForErrors("could not get v_xyzgEyePosW_wDoMotionBlur parameter\n");
    
	//f_xyzgEyePosW_wDoMotionBlur = glGetUniformLocation(pEffect->m_glslProgram, "xyzgEyePosW_wDoMotionBlur");
	//NSLog(@"f_xyzgEyePosW_wDoMotionBlur uniform location: %d", f_xyzgEyePosW_wDoMotionBlur);
    //IRenderer::checkForErrors("could not get f_xyzgEyePosW_wDoMotionBlur parameter\n");
    
#else
	v_gViewProj = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gViewProj");
	IRenderer::checkForErrors("could not get gViewProj parameter\n");

	f_gViewProj = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gViewProj");
	IRenderer::checkForErrors("could not get gViewProj parameter\n");

	v_gPreviousViewProjMatrix = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gPreviousViewProjMatrix");
	IRenderer::checkForErrors("could not get gPreviousViewProjMatrix parameter\n");

	f_gPreviousViewProjMatrix = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gPreviousViewProjMatrix");
	IRenderer::checkForErrors("could not get gPreviousViewProjMatrix parameter\n");

	v_gViewProjInverseMatrix = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gViewProjInverseMatrix");
	IRenderer::checkForErrors("could not get gViewProjInverseMatrix parameter\n");

	f_gViewProjInverseMatrix = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gViewProjInverseMatrix");
	IRenderer::checkForErrors("could not get gViewProjInverseMatrix parameter\n");

	v_gViewInv = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gView");
	IRenderer::checkForErrors("could not get gViewInv parameter\n");

	f_gViewInv = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gView");
	IRenderer::checkForErrors("could not get gViewInv parameter\n");

	v_gLightWVP = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gLightWVP");
	IRenderer::checkForErrors("could not get gLightWVP parameter\n");

	f_gLightWVP = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gLightWVP");
	IRenderer::checkForErrors("could not get gLightWVP parameter\n");

	v_gEyePosW = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gEyePosW");
	IRenderer::checkForErrors("could not get gEyePosW parameter\n");

	f_gEyePosW = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gEyePosW");
	IRenderer::checkForErrors("could not get gEyePosW parameter\n");

	v_xyzgEyePosW_wDoMotionBlur = cgGetNamedParameter(pEffect->m_cgVertexProgram, "xyzgEyePosW_wDoMotionBlur");
	IRenderer::checkForErrors("could not get xyzgEyePosW_wDoMotionBlur parameter\n");

	f_xyzgEyePosW_wDoMotionBlur = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "xyzgEyePosW_wDoMotionBlur");
	IRenderer::checkForErrors("could not get xyzgEyePosW_wDoMotionBlur parameter\n");
#endif
    
    for (unsigned int i = 0; i < NUM_LIGHT_SOURCES; i++)
	{
		initializeLightCGParams(i, v_gLight[i], f_gLight[i], pEffect);
	}
}

void SetInstanceControlConstantsShaderAction::PerEffectBindIds::initializeLightCGParams(unsigned int index, LightCGParams &out_v, LightCGParams &out_f, Effect *pEffect)
{
#if APIABSTRACTION_IOS
    char index_num[8];
	char *name_index = new char[80]; //"gLight";
	strcpy(name_index, "gLight");
	char *name = new char[80];
    
	sprintf(index_num, "%u", index);
	strcat(name_index, index_num); //"gLightX"
	strcpy(name, name_index);
    
	strcat(name, ".xyzPos_w"); //"gLightX.xyzPos_w"
	out_f.pos = out_v.pos = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.pos = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.pos);
    IRenderer::checkForErrors("could not get gLight.xyzPos parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".xyzDir_w"); //"gLightX.xyzDir_w"
	out_f.dir = out_v.dir = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.dir = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.dir);
    IRenderer::checkForErrors("could not get gLight.xyzDir parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".ambient"); //"gLightX.ambient"
	out_f.ambient = out_v.ambient = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.ambient = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.ambient);
    IRenderer::checkForErrors("could not get gLight.ambient parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".diffuse"); //"gLightX.diffuse"
	out_f.diffuse = out_v.diffuse = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.diffuse = glGetUniformLocation(pEffect->m_cgFragmentProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.diffuse);
    IRenderer::checkForErrors("could not get gLight.diffuse parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".spec"); //"gLightX.spec"
	out_f.specular = out_v.specular = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.specular = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.specular);
    IRenderer::checkForErrors("could not get gLight.specular parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".xyzAtt_wSpotPwr"); //"gLightX.xyzAtt_wSpotPwr"
	out_f.atten_spot = out_v.atten_spot = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.atten_spot = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.atten_spot);
    IRenderer::checkForErrors("could not get gLight.AttSpot parameter\n");
    
	strcpy(name, name_index);
	strcat(name, ".xRange_yType_zw"); //"gLightX.xRange_yType_zw"
	out_f.range_type = out_v.range_type = glGetUniformLocation(pEffect->m_glslProgram, name);
	//out_f.range_type = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, out_v.range_type);
    IRenderer::checkForErrors("could not get gLight.RangeType parameter\n");
    
	delete[] name;
	delete[] name_index;
#else
    char index_num[8];
	char *name_index = new char[80]; //"gLight";
	strcpy(name_index, "gLight");
	char *name = new char[80];

	sprintf(index_num, "%u", index);
	strcat(name_index, index_num); //"gLightX"
	strcpy(name, name_index);

	strcat(name, ".xyzPos_w"); //"gLightX.xyzPos_w"
	out_v.pos = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.pos = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.xyzPos parameter\n");

	strcpy(name, name_index);
	strcat(name, ".xyzDir_w"); //"gLightX.xyzDir_w"
	out_v.dir = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.dir = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.xyzDir parameter\n");

	strcpy(name, name_index);
	strcat(name, ".ambient"); //"gLightX.ambient"
	out_v.ambient = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.ambient = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.ambient parameter\n");

	strcpy(name, name_index);
	strcat(name, ".diffuse"); //"gLightX.diffuse"
	out_v.diffuse = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.diffuse = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.diffuse parameter\n");

	strcpy(name, name_index);
	strcat(name, ".spec"); //"gLightX.spec"
	out_v.specular = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.specular = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.specular parameter\n");

	strcpy(name, name_index);
	strcat(name, ".xyzAtt_wSpotPwr"); //"gLightX.xyzAtt_wSpotPwr"
	out_v.atten_spot = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.atten_spot = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.AttSpot parameter\n");

	strcpy(name, name_index);
	strcat(name, ".xRange_yType_zw"); //"gLightX.xRange_yType_zw"
	out_v.range_type = cgGetNamedParameter(pEffect->m_cgVertexProgram, name);
	out_f.range_type = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get gLight.RangeType parameter\n");

	delete[] name;
	delete[] name_index;
#endif    
}
#endif

#endif
}; // namespace PE
