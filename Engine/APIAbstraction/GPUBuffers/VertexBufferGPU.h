#ifndef __PYENGINE_2_0_VERTEX_BUFFER_GPU__
#define __PYENGINE_2_0_VERTEX_BUFFER_GPU__
// This class works with vertex buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "PrimeEngine/Geometry/TangentBufferCPU/TangentBufferCPU.h"
#include "PrimeEngine/Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "PrimeEngine/Geometry/ParticleSystemCPU/ParticleBufferCPU.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkinWeightsCPU.h"

#include "PrimeEngine/Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "PrimeEngine/Geometry/ColorBufferCPU.h"
#if APIABSTRACTION_D3D9
#include "PrimeEngine/APIAbstraction/DirectX9/D3D9_GPUBuffers/D3D9_VertexBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "PrimeEngine/APIAbstraction/DirectX11/D3D11_GPUBuffers/D3D11_VertexBufferGPU.h"
#elif APIABSTRACTION_OGL
#include "PrimeEngine/APIAbstraction/OGL/OGL_GPUBuffers/OGL_VertexBufferGPU.h"
#elif PE_PLAT_IS_PSVITA

#endif

#include "IndexBufferGPU.h"
#include "BufferInfo.h"

// Sibling/Children includes
namespace PE {
namespace Components
{
struct Effect;
}



class VertexBufferGPU : public PE::PEAllocatableAndDefragmentable
{
private:
	enum WRITE_MODES
	{
		CONSTANT,
		WRITABLE_BY_API,
		STREAM_OUTPUT,
	};
	///
	/// Create a gpu buffer from a PositionBufferCPU. The input buffer can contain any number of bytes per vertex (= vertexSize)
	/// Function assumes array of 32 bit floats and 3 
	///
	void internalCreateGPUBufferFromCombined(PositionBufferCPU &vb, PrimitiveTypes::UInt32 vertexSize, WRITE_MODES writeMode = CONSTANT);

public:

	VertexBufferGPU(PE::GameContext &context, PE::MemoryArena arena);

	~VertexBufferGPU();

	
	void createGPUBufferFromSize(PrimitiveTypes::UInt32 stride, PrimitiveTypes::UInt32 numStrides, WRITE_MODES writeMode = CONSTANT);
	
	void createStreamOutputGPUBuffer(PrimitiveTypes::UInt32 maxSize, PrimitiveTypes::UInt32 stride);


	void createGPUBuffer(PositionBufferCPU &vb, TexCoordBufferCPU &tcb,  TexCoordBufferCPU &tcb1);

	void createGPUBufferFromSource_ColoredMinimalMesh(PositionBufferCPU &vb, ColorBufferCPU &cb, WRITE_MODES writeMode = CONSTANT);
	void createGPUBufferFromSource_ReducedMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, WRITE_MODES writeMode = CONSTANT);
	void createGPUBufferFromSource_StdMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, NormalBufferCPU &nb);
	void createGPUBufferFromSource_DetailedMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, NormalBufferCPU &nb, TangentBufferCPU &tb);
	
	void createGPUBufferFromSource_MinimalSkin(PositionBufferCPU &vb, SkinWeightsCPU &weights);
	void createGPUBufferFromSource_ReducedSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights);
	void createGPUBufferFromSource_StdSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights, NormalBufferCPU &nb);
	void createGPUBufferFromSource_DetailedSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights, NormalBufferCPU &nb, TangentBufferCPU &tb);

	void createGPUBuffer(PositionBufferCPU &vb, NormalBufferCPU &nb);

	template <typename Particle_type>
	void createGPUBuffer(ParticleBufferCPU<Particle_type> &pb)
	{
		PositionBufferCPU combined(*m_pContext, m_arena);
		combined.m_values.reset(pb.getByteSize() / sizeof(PrimitiveTypes::Float32));
		PrimitiveTypes::UInt32 ivbval = 0;
		PrimitiveTypes::UInt32 numFloatsPerParticle = sizeof(Particle_type) / sizeof(PrimitiveTypes::Float32);

		for (PrimitiveTypes::UInt32 ip = 0; ip < pb.m_values.m_size; ip++)
		{
			for (PrimitiveTypes::UInt32 i = 0; i < numFloatsPerParticle; i++)
				combined.m_values.add(0.0f); // allocate memory

			memcpy(&combined.m_values[ivbval], &pb.m_values[ip], sizeof(Particle_type));

			ivbval += numFloatsPerParticle;
		}

#		if APIABSTRACTION_D3D9
		// will use D3D9_VertexBufferGPU

		// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
		m_pBuf = D3D9_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			IRenderer::Instance()->getDevice(), combined.getStartAddress(), combined.getByteSize());
		m_vertexSize = sizeof(Particle_type);
		m_length = pb.m_values.m_size;
#		elif APIABSTRACTION_D3D11
		// will use D3D11_VertexBufferGPU

		// retrieve device from IRenderer
		ID3D11Device *pDevice = IRenderer::Instance()->m_device;

		// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
		m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, combined.getStartAddress(), combined.getByteSize());
		m_vertexSize = sizeof(Particle_type);
		m_length = pb.m_values.m_size;
#		endif
	}

	void setAPIValues();

#if APIABSTRACTION_OGL && OGL_USE_VERTEX_BUFFER_ARRAYS
	void linkIndexBuffer(IndexBufferGPU &ibgpu)
	{
		// Bind our Vertex Array Object as the current used object
		glBindVertexArray(m_buf);

		// bind index buffer of current VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibgpu.m_buf);
	}
#endif

	void setAsCurrentStreamOutput()
	{
#		if APIABSTRACTION_D3D9
			assert(!"Not Implemented");
#		elif APIABSTRACTION_D3D11
			D3D11Renderer *pRenderer = static_cast<D3D11Renderer*>(m_pContext->getGPUScreen());
			ID3D11Device *pDevice = pRenderer->m_pD3DDevice;
			ID3D11DeviceContext *pDeviceContext = pRenderer->m_pD3DContext;
			ID3D11Buffer *pBuf = m_pBuf;
			PrimitiveTypes::UInt32 offset = 0;
			pDeviceContext->SOSetTargets(1, &pBuf, &offset);
#		endif
	}

	static void UnbindVertexBufferStreamOutputs(PE::GameContext &ctx )
	{
#		if APIABSTRACTION_D3D9
		assert(!"Not Implemented");
#		elif APIABSTRACTION_D3D11
			D3D11Renderer *pRenderer = static_cast<D3D11Renderer*>(ctx.getGPUScreen());
			ID3D11Device *pDevice = pRenderer->m_pD3DDevice;
			ID3D11DeviceContext *pDeviceContext = pRenderer->m_pD3DContext;
			ID3D11Buffer *bufferArray[1] = {0};
			PrimitiveTypes::UInt32 offset = 0;
			pDeviceContext->SOSetTargets(1, bufferArray, &offset);
#		endif
	}

	void setAsCurrent(PE::Components::Effect *pEffect);
	void unbindFromPipeline(PE::Components::Effect *pEffect);

	static void setAsCurrent(PE::GameContext &context, PE::MemoryArena arena, Array<Handle> &hBuffersGPU);

	void *mapToPtr();
	void releasePtrAndCopyBack();

	void releaseGPUBuffer();
#if APIABSTRACTION_D3D11
	static void *s_mapToPtr(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pSrcBuf, ID3D11Buffer * &tmpBuf);
	static void s_releasePtrAndCopyBack(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pSrcBuf, ID3D11Buffer * tmpBuf);
#endif

	void draw()
	{
		if (!m_isStreamOutput)
		{
#			if APIABSTRACTION_D3D9
				assert(!"Should Be Implemented!");
#			elif APIABSTRACTION_D3D11
				D3D11Renderer *pRenderer = static_cast<D3D11Renderer*>(m_pContext->getGPUScreen());
				ID3D11Device *pDevice = pRenderer->m_pD3DDevice;
				ID3D11DeviceContext *pDeviceContext = pRenderer->m_pD3DContext;

				pDeviceContext->Draw(m_length, 0);
#			endif
		}
		else
		{

#			if APIABSTRACTION_D3D9
				assert(!"Should Be Implemented");
#			elif APIABSTRACTION_D3D11
				D3D11Renderer *pRenderer = static_cast<D3D11Renderer*>(m_pContext->getGPUScreen());
				ID3D11DeviceContext *pDeviceContext = pRenderer->m_pD3DContext;
				pDeviceContext->DrawAuto();
#			endif
		}
	}


	static const int MAX_BUFFERS = 16;
#if APIABSTRACTION_D3D9
		IDirect3DVertexBuffer9 *m_pBufs[MAX_BUFFERS];
		IDirect3DVertexBuffer9 *m_pBuf;
#elif APIABSTRACTION_D3D11
		ID3D11Buffer *m_pBufs[MAX_BUFFERS];
		ID3D11Buffer *m_pBuf;
#elif APIABSTRACTION_OGL
		GLuint m_buf;
		GLuint m_bufs[MAX_BUFFERS];
#elif PE_PLAT_IS_PSVITA
		SceUID m_basicVerticesUid;
		void* m_basicVertices;
#endif

	PEVertexBufferInfo *m_pBufferSetInfo;

	/// number of vertices in the buffer
	PrimitiveTypes::UInt32 m_length;

	PrimitiveTypes::UInt32 m_vertexSize; ///< size of one vertex data in bytes. example sizeof(float32) * (3 + 2) - position + tex coords 
	
	PrimitiveTypes::Bool m_isStreamOutput;
#if APIABSTRACTION_D3D11
	ID3D11Buffer *m_pTmpBuffer;
#endif
	PrimitiveTypes::Float32 m_weight;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

}; // namespace PE

#endif
