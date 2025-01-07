// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"


// Sibling/Children includes
#include "SetPerObjectConstantsShaderAction.h"
namespace PE {
using namespace Components;

void SetPerObjectConstantsShaderAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#if PE_PLAT_IS_PSVITA
#	elif APIABSTRACTION_D3D11

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	Effect::setConstantBuffer(pDevice, pDeviceContext, s_pBuffer, 2, &m_data, sizeof(Data));
#	elif APIABSTRACTION_D3D9
	//assert(sizeof(m_data) == 77 * 4 * sizeof(float)); // the data is synced to take 78 float4 registers on GPU
    
    int dataNoBones = sizeof(m_data) - sizeof(m_data.gJoints);
    int dataBones = sizeof(m_data);
    
    int dataTouse = m_useBones ? dataBones : dataNoBones;
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
    HRESULT hr = pDevice->SetVertexShaderConstantF(78, (const float *)(&m_data), dataTouse / (4*sizeof(float)));
	assert(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(78, (const float *)(&m_data), dataTouse / (4*sizeof(float)));
	assert(SUCCEEDED(hr));
#elif APIABSTRACTION_OGL
	if (pCurEffect)
	{
		ExternalPerTechniqueData &data = pCurEffect->m_externalPerTechniqueData;
	
        #if APIABSTRACTION_IOS

        glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.v_gWVP, 1, GL_FALSE,
                                 (const GLfloat*)(&m_data.gWVP));
        IRenderer::checkForErrors("setting parameter values\n");
        
        //glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.f_gWVP, 1, GL_FALSE,
        //                         (const GLfloat*)(&m_data.gWVP));
        //IRenderer::checkForErrors("setting parameter values\n");
        
        glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.v_gW, 1, GL_FALSE,
                                 (const GLfloat*)(&m_data.gW));
        IRenderer::checkForErrors("setting parameter values\n");
        
        //glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.f_gW, 1, GL_FALSE,
        //                         (const GLfloat*)(&m_data.gW));
        //IRenderer::checkForErrors("setting parameter values\n");
        
        glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.v_gUnused, 1, GL_FALSE,
                                 (const GLfloat*)(&m_data.gUnused));
        IRenderer::checkForErrors("setting parameter values\n");
        
        //glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.f_gUnused, 1, GL_FALSE,
        //                         (const GLfloat*)(&m_data.gUnused));
        //IRenderer::checkForErrors("setting parameter values\n");
        
        glUniform4fv(data.m_cbPerObject_c2_cgparameters.v_gVertexBufferWeights, 1,
                            (const GLfloat*)(&m_data.gVertexBufferWeights));
        IRenderer::checkForErrors("setting parameter values\n");
        
        //glUniform4fv(data.m_cbPerObject_c2_cgparameters.f_gVertexBufferWeights, 1,
        //                    (const GLfloat*)(&m_data.gVertexBufferWeights));
        //IRenderer::checkForErrors("setting parameter values\n");
        
        glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.v_gWVPInverse, 1, GL_FALSE,
                                 (const GLfloat*)(&m_data.gWVPInverse));
        IRenderer::checkForErrors("setting parameter values\n");
        
        //glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.f_gWVPInverse, 1, GL_FALSE,
        //                         (const GLfloat*)(&m_data.gWVPInverse));
        //IRenderer::checkForErrors("setting parameter values\n");
        
        
        if (m_useBones)
        {
            glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.v_gJoints, sizeof(m_data.gJoints)/sizeof(Matrix4x4), GL_FALSE,
                                      (const GLfloat*)(&m_data.gJoints[0]));
            IRenderer::checkForErrors("setting parameter values\n");
        }
        
        
        //glUniformMatrix4fv(data.m_cbPerObject_c2_cgparameters.f_gJoints, sizeof(m_data.gJoints)/sizeof(Matrix4x4), GL_FALSE,
        //                   (const GLfloat*)(&m_data.gJoints[0]));
        //IRenderer::checkForErrors("setting parameter values\n");

        #else
		    cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.v_gWVP,
                (const GLfloat*)(&m_data.gWVP));//cgSetParameterValuefc(v_gWVP, sizeof(m_data) / sizeof(float), (const GLfloat*)(&m_data.gWVP));
            IRenderer::checkForErrors("setting v_gWVP parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.f_gWVP,
			(const GLfloat*)(&m_data.gWVP)); //cgSetParameterValuefc(f_gWVP, sizeof(m_data) / sizeof(float), (const GLfloat*)(&m_data.gWVP));
            IRenderer::checkForErrors("setting f_gWVP parameter values\n");

			    
            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.v_gW,
			(const GLfloat*)(&m_data.gW));
            IRenderer::checkForErrors("setting v_gW parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.f_gW,
			(const GLfloat*)(&m_data.gW));
            IRenderer::checkForErrors("setting f_gW parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.v_gUnused,
			(const GLfloat*)(&m_data.gUnused));
            IRenderer::checkForErrors("setting v_gUnused parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.f_gUnused,
			(const GLfloat*)(&m_data.gUnused));
            IRenderer::checkForErrors("setting f_gUnused parameter values\n");

            cgGLSetParameter4fv(data.m_cbPerObject_c2_cgparameters.v_gVertexBufferWeights,
			(const GLfloat*)(&m_data.gVertexBufferWeights));
            IRenderer::checkForErrors("setting v_gVertexBufferWeights parameter values\n");

            cgGLSetParameter4fv(data.m_cbPerObject_c2_cgparameters.f_gVertexBufferWeights,
			(const GLfloat*)(&m_data.gVertexBufferWeights));
            IRenderer::checkForErrors("setting f_gVertexBufferWeights parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.v_gWVPInverse,
			(const GLfloat*)(&m_data.gWVPInverse));
            IRenderer::checkForErrors("setting v_gWVPInverse parameter values\n");

            cgGLSetMatrixParameterfc(data.m_cbPerObject_c2_cgparameters.f_gWVPInverse,
			(const GLfloat*)(&m_data.gWVPInverse));
            IRenderer::checkForErrors("setting f_gWVPInverse parameter values\n");

            if (m_useBones)
			{
				
				/*
				int dim = cgGetArrayDimension( data.m_cbPerObject_c2_cgparameters.v_gJoints );
				int arrSize = cgGetArraySize(data.m_cbPerObject_c2_cgparameters.v_gJoints, 0 );
				*/

                cgGLSetMatrixParameterArrayfc(data.m_cbPerObject_c2_cgparameters.v_gJoints, 0, sizeof(m_data.gJoints)/sizeof(Matrix4x4),
                                              (const GLfloat*)(&m_data.gJoints[0]));
                IRenderer::checkForErrors("setting v_gJoints parameter values\n");

				


            }
        #endif
        
			//cgGLUpdateProgramParameters(pCurEffect->m_cgVertexProgram);
            //cgGLUpdateProgramParameters(pCurEffect->m_cgFragmentProgram);
        #if APIABSTRACTION_GLPC
            //cgGLUpdateProgramParameters(pCurEffect->m_cgVertexProgram);
            //cgGLUpdateProgramParameters(pCurEffect->m_cgFragmentProgram);
        #endif

	}
#	endif
}

void SetPerObjectConstantsShaderAction::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{}

void SetPerObjectConstantsShaderAction::releaseData()
{}

#if APIABSTRACTION_OGL
void SetPerObjectConstantsShaderAction::PerEffectBindIds::initialize(Effect *pEffect)
{
#if APIABSTRACTION_IOS 
    
    v_gWVP = glGetUniformLocation(pEffect->m_glslProgram, "gWVP");
    NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	//f_gWVP = glGetUniformLocation(pEffect->m_glslProgram, "gWVP");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	v_gW = glGetUniformLocation(pEffect->m_glslProgram, "gW");
	NSLog(@"gW uniform location: %d", v_gW);
    
	//f_gW = glGetUniformLocation(pEffect->m_glslProgram, "gW");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	v_gUnused = glGetUniformLocation(pEffect->m_glslProgram, "gUnused");
	NSLog(@"gUnused uniform location: %d", v_gUnused);
    
	//f_gUnused = glGetUniformLocation(pEffect->m_glslProgram, "gUnused");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	v_gVertexBufferWeights = glGetUniformLocation(pEffect->m_glslProgram, "gVertexBufferWeights");
	NSLog(@"gVertexBufferWeights uniform location: %d", v_gVertexBufferWeights);
    
	//f_gVertexBufferWeights = glGetUniformLocation(pEffect->m_glslProgram, "gVertexBufferWeights");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	v_gWVPInverse = glGetUniformLocation(pEffect->m_glslProgram, "gWVPInverse");
	NSLog(@"gWVPInverse uniform location: %d", v_gWVPInverse);
    
	//f_gWVPInverse = glGetUniformLocation(pEffect->m_glslProgram, "gWVPInverse");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
	v_gJoints = glGetUniformLocation(pEffect->m_glslProgram, "gJoints");
	NSLog(@"gJoints uniform location: %d", v_gJoints);
    
	//f_gJoints = glGetUniformLocation(pEffect->m_glslProgram, "gJoints");
	//NSLog(@"gWVP uniform location: %d", v_gWVP);
    
#else
	v_gWVP = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gWVP");
	IRenderer::checkForErrors("could not get gWVP parameter\n");

	f_gWVP = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gWVP");
	IRenderer::checkForErrors("could not get gWVP parameter\n");

	v_gW = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gW");
	IRenderer::checkForErrors("could not get gW parameter\n");

	f_gW = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gW");
	IRenderer::checkForErrors("could not get gW parameter\n");

	v_gUnused = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gUnused");
	IRenderer::checkForErrors("could not get gUnused parameter\n");

	f_gUnused = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gUnused");
	IRenderer::checkForErrors("could not get gUnused parameter\n");

	v_gVertexBufferWeights = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gVertexBufferWeights");
	IRenderer::checkForErrors("could not get gVertexBufferWeights parameter\n");

	f_gVertexBufferWeights = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gVertexBufferWeights");
	IRenderer::checkForErrors("could not get gVertexBufferWeights parameter\n");

	v_gWVPInverse = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gWVPInverse");
	IRenderer::checkForErrors("could not get gVWPInverse parameter\n");

	f_gWVPInverse = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gWVPInverse");
	IRenderer::checkForErrors("could not get gVWPInverse parameter\n");

	v_gJoints = cgGetNamedParameter(pEffect->m_cgVertexProgram, "gJoints");
	IRenderer::checkForErrors("could not get gJoints parameter\n");

	f_gJoints = cgGetNamedParameter(pEffect->m_cgFragmentProgram, "gJoints");
	IRenderer::checkForErrors("could not get gJoints parameter\n");
#endif
}
#endif

}; // namespace PE
