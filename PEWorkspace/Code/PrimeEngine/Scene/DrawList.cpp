#include "DrawList.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/ResourceBufferGPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/IndexBufferGPU.h"
#include "PrimeEngine/APIAbstraction/Timer/Timer.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/ParticleSetBufferGPU.h"
#include "../Lua/LuaEnvironment.h"
#include "../Profiling/Profiling.h"

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(DrawList, Component);

Handle DrawList::s_normalLists[2] = {Handle(), Handle()};
Handle DrawList::s_zOnlyLists[2] = {Handle(), Handle()};
PrimitiveTypes::UInt32 DrawList::m_curBuffer = 0;

void DrawList::importMesh(Handle hMesh)
{
	m_objects.add(hMesh);
}

void DrawList::importSkin(Handle hSkin)
{
	m_objects.add(hSkin);
}

void DrawList::reset()
{
	m_objects.reset(c_maxNumDrawCalls);
	m_dbgNames.reset(c_maxNumDrawCalls);
	m_effects.reset(c_maxNumDrawCalls);

	for (PrimitiveTypes::UInt32 isv = 0; isv < m_vertexBuffers.m_size; isv++)
	{
		Array<Handle> &arr = m_vertexBuffers[isv];
		arr.reset(0);
	}

	m_vertexBuffers.reset(c_maxNumDrawCalls);
	
	m_instanceVertexBuffers.reset(c_maxNumDrawCalls);
	m_outputs.reset(c_maxNumDrawCalls);
	m_indexBuffers.reset(c_maxNumDrawCalls);
	m_indexRanges.reset(c_maxNumDrawCalls);
	m_boneSegmentIds.reset(c_maxNumDrawCalls);
	m_instanceCounts.reset(c_maxNumDrawCalls);
	m_instanceOffsets.reset(c_maxNumDrawCalls);
	m_dispatchParams.reset(c_maxNumDrawCalls);
	m_drawCallTypes.reset(c_maxNumDrawCalls);
	
	for (PrimitiveTypes::UInt32 isv = 0; isv < m_globalShaderValues.m_size; isv++)
	{
		ShaderAction *sv = m_globalShaderValues[isv].getObject<ShaderAction>();
		sv->releaseData();
		m_globalShaderValues[isv].release();
	}
	m_globalShaderValues.reset(c_maxNumDrawCalls);

	for (PrimitiveTypes::UInt32 icall = 0; icall < m_numDrawCalls; icall++)
	{
		Array<Handle> &shaderValues = m_shaderValues[icall];
		for (PrimitiveTypes::UInt32 isv = 0; isv < shaderValues.m_size; isv++)
		{
			ShaderAction *sv = shaderValues[isv].getObject<ShaderAction>();
			sv->releaseData();
			shaderValues[isv].release();
		}
		shaderValues.reset(0);
	}

	m_shaderValues.reset(c_maxNumDrawCalls);
	m_numDrawCalls = 0;
}

void DrawList::beginDrawCallRecord(const char *dbgName)
{
	m_numDrawCalls++;
	m_dbgNames.add(dbgName);
	m_effects.add(Handle());
	m_vertexBuffers.add(Array<Handle>(*m_pContext, m_arena, 4));
	m_instanceVertexBuffers.add(Handle());
	m_indexBuffers.add(Handle());
	m_indexRanges.add(-1);
	m_boneSegmentIds.add(-1);
	m_instanceCounts.add(1);
	m_instanceOffsets.add(0);
	m_dispatchParams.add(Vector3());
	m_outputs.add(Handle());
	m_shaderValues.add(Array<Handle>(*m_pContext, m_arena));
	m_shaderValues[m_shaderValues.m_size-1].reset(16);

	m_drawCallTypes.add(DrawCallType::VBUF_INDBUF);
}

void DrawList::setEffect(Handle hEffect) {
	
	m_effects[m_numDrawCalls-1] = hEffect;
	PEASSERT(hEffect.isValid(), "Invalid hanle passed in as effect");
}
void DrawList::setIndexBuffer(Handle hIndBuf, PrimitiveTypes::Int32 indBufRange, PrimitiveTypes::Int32 boneSegmentId)
{
	m_indexBuffers[m_numDrawCalls-1] = hIndBuf;
	m_indexRanges[m_numDrawCalls-1] = indBufRange;
	m_boneSegmentIds[m_numDrawCalls-1] = boneSegmentId;
}

void DrawList::setInstanceCount(PrimitiveTypes::UInt32 count, PrimitiveTypes::UInt32 instanceOffset)
{
	m_instanceCounts[m_numDrawCalls-1] = count;
	m_instanceOffsets[m_numDrawCalls-1] = instanceOffset;
}

void DrawList::setDrawIndirect() {
	m_drawCallTypes[m_numDrawCalls - 1] = DrawCallType::VBUF_INDBUF_INDIRECT;
}

void DrawList::setDispatchIndirect() {
	m_drawCallTypes[m_numDrawCalls - 1] = DrawCallType::COMPUTE_INDIRECT;
}

void DrawList::setVertexBuffer(Handle hVertBuf)
{
	if (hVertBuf.isValid())
	{
		m_vertexBuffers[m_numDrawCalls-1].add(hVertBuf);
	}
	else
	{
		m_drawCallTypes[m_numDrawCalls-1] = DrawCallType::COMPUTE;
	}
}

void DrawList::setDispatchParams(Vector3 dispatch)
{
	m_dispatchParams[m_numDrawCalls-1] = dispatch;
}

void DrawList::setInstanceVertexBuffer(Handle hInstVertBuf) {m_instanceVertexBuffers[m_numDrawCalls-1] = hInstVertBuf;}
void DrawList::setStreamOutputVertexBuffer(Handle hVertBuf) {m_outputs[m_numDrawCalls-1] = hVertBuf;}

Handle &DrawList::nextShaderValue()
{
	Array<Handle> &shaderValues = m_shaderValues[m_numDrawCalls-1];
	shaderValues.add(Handle());
	return shaderValues[shaderValues.m_size-1];
}

Handle &DrawList::nextShaderValue(int size)
{
	Array<Handle> &shaderValues = m_shaderValues[m_numDrawCalls-1];
	shaderValues.add(Handle("RAW_DATA", size));
	return shaderValues[shaderValues.m_size-1];
}

Handle &DrawList::nextGlobalShaderValue()
{
	// this handle will be returned by reference and filled in by user
	m_globalShaderValues.add(Handle());
	return m_globalShaderValues[m_globalShaderValues.m_size-1];
}

void DrawList::optimize()
{
	m_optimizedIndices.reset(m_numDrawCalls);
#if 1 // disable optimization
	for (PrimitiveTypes::UInt32 i = 0; i < m_numDrawCalls; i++)
	{
		m_optimizedIndices.add(i);
	}
	return;
#else

	Array<Handle> effects(*m_pContext, m_arena, m_effects.m_size);
	
	// sort by effects
	Array<PrimitiveTypes::Bool> processed(*m_pContext, m_arena, m_numDrawCalls);
	for (PrimitiveTypes::UInt32 i = 0; i < m_numDrawCalls; i++)
	{
		processed.add(false);
	}

	for (PrimitiveTypes::UInt32 i = 0; i < m_numDrawCalls; i++)
	{
		processed.add(false);
	}
	Array<PrimitiveTypes::UInt32> indicesSameEffect(*m_pContext, m_arena, m_numDrawCalls);

	for (PrimitiveTypes::UInt32 icall = 0; icall < m_numDrawCalls; icall++)
	{
		if (processed[icall]) 
			continue;
		Handle heffect = m_effects[icall];
		effects.add(heffect);
		processed[icall] = true;
		indicesSameEffect.m_size = 0;
		indicesSameEffect.add(icall);
		for (PrimitiveTypes::UInt32 jcall = icall+1; jcall < m_numDrawCalls; jcall++)
		{
			if (!processed[jcall])
			{
				Handle hother = m_effects[jcall];
				if (hother == heffect)
				{
					// same effect
					effects.add(Handle());
					indicesSameEffect.add(jcall);
					processed[jcall] = true;
					m_effects[jcall] = Handle(); // one has been set already
				}
			}
		}
		// put bp here..
		// dbg
        //Effect *pE = heffect.getObject<Effect>();
		
        // now have a list of call indices for the same effect
		optimizeByVertexBuffer(indicesSameEffect);


		for (PrimitiveTypes::UInt32 i =0; i < indicesSameEffect.m_size; i++)
		{
			//dbg
            //PrimitiveTypes::UInt32 call = indicesSameEffect[i];
			
            m_optimizedIndices.add(indicesSameEffect[i]);
		}
	}

	effects.reset(0);
	processed.reset(0);
	indicesSameEffect.reset(0);
#endif
}

void DrawList::optimizeByVertexBuffer(Array<PrimitiveTypes::UInt32> &callIndices)
{
	Array<PrimitiveTypes::Bool> processed(*m_pContext, m_arena, callIndices.m_size);
	for (PrimitiveTypes::UInt32 i = 0; i < callIndices.m_size; i++)
	{
		processed.add(false);
	}
	Array<PrimitiveTypes::UInt32> indicesSameVB(*m_pContext, m_arena, callIndices.m_size);

	Array<PrimitiveTypes::UInt32> resCallIndices(*m_pContext, m_arena, callIndices.m_size);

	for (PrimitiveTypes::UInt32 i = 0; i < callIndices.m_size; i++)
	{
		if (processed[i])
			continue;
		processed[i] = true;
		PrimitiveTypes::UInt32 icall = callIndices[i];
		Array<Handle> &hvb = m_vertexBuffers[icall];
		indicesSameVB.m_size = 0;
		indicesSameVB.add(icall);
		for (PrimitiveTypes::UInt32 j = i+1; j < callIndices.m_size; j++)
		{
			if (!processed[j])
			{
				PrimitiveTypes::UInt32 jcall = callIndices[j];
				Array<Handle> &hother = m_vertexBuffers[jcall];
				bool equal = true;
				if (hother.m_size == hvb.m_size)
				{
					for (PrimitiveTypes::UInt32 ih = 0; ih < hvb.m_size; ih++)
					{
						if (!(hvb[ih] == hother[ih]))
						{
							equal = false;
							break;
						}
					}
				}
				else
				{
					equal = false;
				}
				if (equal)
				{
					processed[j] = true;
					indicesSameVB.add(jcall);
					//m_vertexBuffers[jcall].reset(0);
				}
			}
		}

		// now have a list of drw call with the same vertex buffer (withing given set of indices i.e. for the same effect)
		// TODO: sort it more..
		for (PrimitiveTypes::UInt32 k = 0; k < indicesSameVB.m_size; k++)
		{
			//dbg
            //PrimitiveTypes::UInt32 call = indicesSameVB[k];
			
            resCallIndices.add(indicesSameVB[k]);
		}
	}

	for (PrimitiveTypes::UInt32 i = 0; i < callIndices.m_size; i++)
	{
		callIndices[i] = resCallIndices[i];
	}

	indicesSameVB.reset(0);
	resCallIndices.reset(0);
	processed.reset(0);
}

void DrawList::do_RENDER(Events::Event *pEvt, int &threadOwnershipMask)
{
	m_pContext->getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

	m_pCurIndBuf = 0;
	m_pCurEffect = 0;
	m_pCurVertBuf = 0;
	PrimitiveTypes::Int32 curIndBufRange = -1;
	PrimitiveTypes::Int32 curIndBufRangeBoneSegment = -1;
	m_pCurOutput = 0;
	DrawCallType::DrawCallType_ curDrawCallType = DrawCallType::VBUF;
	PEPrimitveTopology curTopology = PEPrimitveTopology_TRIANGLES;
	PrimitiveTypes::UInt32 curInstanceCount = 1;
	PrimitiveTypes::UInt32 curInstanceOffset = 0;

    PE::IRenderer::checkForErrors("");

#	if APIABSTRACTION_D3D9
		//D3D9Renderer::Instance()->m_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#	elif APIABSTRACTION_D3D11
	
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#	endif

	// set constant buffers; note the check sv.m_bufferId != -1 -> we are only setting buffers, not values trhoigh D3DXEffect framework
#	if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || PE_PLAT_IS_PSVITA
		//since on win32 we hardocde shader value to register # relationship
		//we can do this once before all other draw calls
		// for CG we have to do it per effect
	for (PrimitiveTypes::UInt32 isv = 0; isv < m_globalShaderValues.m_size; isv++)
	{
		ShaderAction *sv = m_globalShaderValues[isv].getObject<ShaderAction>();
		sv->bindToPipeline(m_pCurEffect);
	}
#endif

	IRenderer::checkForErrors("starting to draw\n");

	Effect *pPrevEffect = NULL;

	for (PrimitiveTypes::UInt32 i = 0; i < m_numDrawCalls; i++)
	{
		PrimitiveTypes::UInt32 icall = m_optimizedIndices[i];
		curDrawCallType = m_drawCallTypes[icall];

		Vector3 curDispatchParams = m_dispatchParams[icall];

		const char *dbgName = m_dbgNames[icall];
		Handle &hEffect = m_effects[icall];

		bool newEffect = false;
		pPrevEffect = m_pCurEffect;
		if (hEffect.isValid() && hEffect.getObject<Effect>() != m_pCurEffect)
		{
			m_pCurEffect = hEffect.getObject<Effect>();
			newEffect = true;
		}
#if DEBUG_DRAW_CALLS
		PEINFO("Draw call[%d] DbgName: %s tech %s vs %s ps %s cs %s\n", i, dbgName ? dbgName : "none", m_pCurEffect->m_techName,
			m_pCurEffect->m_vsFilename[0] == '\0' ? "none" : m_pCurEffect->m_vsFilename,
			m_pCurEffect->m_psFilename[0] == '\0' ? "none" : m_pCurEffect->m_psFilename,
			m_pCurEffect->m_csFilename[0] == '\0' ? "none" : m_pCurEffect->m_csFilename
			);
#endif
		if (!m_pCurEffect || !m_pCurEffect->lock())
			continue;

		if (!m_pCurEffect->m_isReady)
		{
			m_pCurEffect->unlock();
			continue;
		}
		
		Array<Handle> &hVertBufGPU = m_vertexBuffers[icall];
		if (hVertBufGPU.m_size == 1)
		{
			m_pCurVertBuf = hVertBufGPU[0].getObject<VertexBufferGPU>();
			m_pCurVertBuf->setAsCurrent(m_pCurEffect);
		}
		else if (hVertBufGPU.m_size > 1)
		{
			m_pCurVertBuf = hVertBufGPU[0].getObject<VertexBufferGPU>();
			VertexBufferGPU::setAsCurrent(*m_pContext, m_arena, hVertBufGPU);
		}
		else
		{
			PEASSERT(curDrawCallType == DrawCallType::COMPUTE || curDrawCallType == DrawCallType::COMPUTE_INDIRECT, 
				"If buffer is invalid, we need this call to be compute");
			if (m_pCurVertBuf)
			{
				m_pCurVertBuf->unbindFromPipeline(pPrevEffect);
				m_pCurVertBuf = NULL;
			}
		}

		Handle &hIndBufGPU = m_indexBuffers[icall];
		curIndBufRange = m_indexRanges[icall];
		curIndBufRangeBoneSegment = m_boneSegmentIds[icall];
		curInstanceCount = m_instanceCounts[icall];
		curInstanceOffset = m_instanceOffsets[icall];

		if (hIndBufGPU.isValid())
		{
			if (hIndBufGPU.getObject<IndexBufferGPU>() != m_pCurIndBuf)
			{
				m_pCurIndBuf = hIndBufGPU.getObject<IndexBufferGPU>();
				m_pCurIndBuf->setAsCurrent();
			}
		}
		else
		{
			PEASSERT(curDrawCallType == DrawCallType::COMPUTE || curDrawCallType == DrawCallType::COMPUTE_INDIRECT,
				"If buffer is invalid, we need this call to be compute");
			if (m_pCurIndBuf)
			{
				m_pCurIndBuf->unbindFromPipeline();
				m_pCurIndBuf = NULL;
			}
		}

		if (newEffect)
		{
			m_pCurEffect->setCurrent(m_pCurVertBuf);
		}

		if (newEffect)
		{
			#if APIABSTRACTION_OGL
			//since on win32 we hardocde shader value to register # relationship
			//we can do this once before all other draw calls
			// for CG we have to do it per effect
			// we need to unbind previous global shader values and bind new ones
			if (pPrevEffect)
			{
				for (PrimitiveTypes::UInt32 isv = 0; isv < m_globalShaderValues.m_size; isv++)
				{
					ShaderAction *sv = m_globalShaderValues[isv].getObject<ShaderAction>();
					sv->unbindFromPipeline(pPrevEffect);
				}
			}

			for (PrimitiveTypes::UInt32 isv = 0; isv < m_globalShaderValues.m_size; isv++)
			{
				ShaderAction *sv = m_globalShaderValues[isv].getObject<ShaderAction>();
				sv->bindToPipeline(m_pCurEffect);
			}
			#endif
		}

		Array<Handle> &shaderValues = m_shaderValues[icall];

		// bind per object constant buffers
		for (PrimitiveTypes::UInt32 isv = 0; isv < shaderValues.m_size; isv++)
		{
			ShaderAction *sv = shaderValues[isv].getObject<ShaderAction>();
			sv->bindToPipeline(m_pCurEffect);
		}

		Handle houtput = m_outputs[icall];
		
		if (houtput.isValid())
			m_pCurOutput = houtput.getObject<VertexBufferGPU>();
		else
			m_pCurOutput = NULL;

		if (m_pCurOutput)
		{
			m_pCurOutput->setAsCurrentStreamOutput();
		}

		#if PE_ENABLE_GPU_PROFILING
		Timer t;
		PE::Profiling::Profiler::Instance()->startEventQuery(Profiling::Group_DrawCalls, IRenderer::Instance()->getDevice(), t.GetTime(), dbgName);
		#endif
		if (curDrawCallType == DrawCallType::VBUF_INDBUF && m_pCurVertBuf && m_pCurIndBuf)
		{
			if (curIndBufRange != -1)
			{
				if (curIndBufRangeBoneSegment != -1)
				{
					m_pCurIndBuf->draw(curIndBufRange, curIndBufRangeBoneSegment, curInstanceCount, curInstanceOffset);
				}
				else
					m_pCurIndBuf->draw(curIndBufRange, curInstanceCount, curInstanceOffset);
			}
			else
			{
				m_pCurIndBuf->draw(curInstanceCount, curInstanceOffset);
			}
		}
		else if (curDrawCallType == DrawCallType::VBUF_INDBUF_INDIRECT && m_pCurVertBuf && m_pCurIndBuf) {
			ID3D11Buffer* drawArgsBuffer = ParticleSetBufferGPU::getDrawArgsBuffer();
            m_pCurIndBuf->drawIndirect(drawArgsBuffer, 0);
		}
		else if (curDrawCallType == DrawCallType::VBUF && m_pCurVertBuf)
		{
			if (m_pCurVertBuf)
				m_pCurVertBuf->draw();
		}
		else if (curDrawCallType == DrawCallType::COMPUTE || curDrawCallType == DrawCallType::COMPUTE_INDIRECT || !m_pCurVertBuf) // in case no vertex buffers were set, must be compute call
		{
#			if APIABSTRACTION_D3D11
#if DEBUG_DRAW_CALLS
				PEINFO("DrawList::do_RENDER(): Dispatch(%d, %d, %d)\n", (int)(curDispatchParams.m_x), (int)(curDispatchParams.m_y), (int)(curDispatchParams.m_z));
#endif

				if (curDrawCallType == DrawCallType::COMPUTE_INDIRECT) {
                    ID3D11Buffer* dispatchArgsBuffer = ParticleSetBufferGPU::getDispatchArgsBuffer();
					pDeviceContext->DispatchIndirect(dispatchArgsBuffer, 0);
				}
				else {
					pDeviceContext->Dispatch(
						(int)(curDispatchParams.m_x),
						(int)(curDispatchParams.m_y),
						(int)(curDispatchParams.m_z));
				}
				
#			else
				PEINFO("PE: WARNING: Trying to dispatch a compute shader, but not running DX11 API");
#			endif
		}
		else
		{
			PEINFO("PE: WARNING: DrawList: Unauthorized draw call: not dispatch but had no vertex buffer bound");
		}

		#if PE_ENABLE_GPU_PROFILING
		Timer::TimeType curTime = t.TickAndGetCurrentTime();
		Profiling::Profiler::Instance()->update(Profiling::Group_DrawCalls, false, false, curTime, t);
		//sprintf(PEString::s_buf, "%f", delta);
		#endif

        PE::IRenderer::checkForErrors("");

		// unbind streamed outputs
		if (m_pCurOutput)
		{
			VertexBufferGPU::UnbindVertexBufferStreamOutputs(*m_pContext);
		}
		else if (m_pCurVertBuf)
		{
			m_pCurVertBuf->unbindFromPipeline(m_pCurEffect);
		}
		
		if (m_pCurIndBuf)
		{
			m_pCurIndBuf->unbindFromPipeline();
		}

		// unbind per object constant buffers
		for (PrimitiveTypes::UInt32 isv = 0; isv < shaderValues.m_size; isv++)
		{
			ShaderAction *sv = shaderValues[isv].getObject<ShaderAction>();
			sv->unbindFromPipeline(m_pCurEffect);
		}
		m_pCurEffect->unlock();
#if DEBUG_DRAW_CALLS
		PEINFO("DrawList::do_RENDER(): End Draw Call\n");
#endif
	}

	//unbind global values
	// in case of DX9/11 where we set global values only once this works
	// this also works for unbinding global values for OGL last effect
	if (m_pCurEffect)
	{
		for (PrimitiveTypes::UInt32 isv = 0; isv < m_globalShaderValues.m_size; isv++)
		{
			ShaderAction *sv = m_globalShaderValues[isv].getObject<ShaderAction>();
			sv->unbindFromPipeline(m_pCurEffect);
		}
	}

	if (m_pCurVertBuf)
	{
		m_pCurVertBuf->unbindFromPipeline(pPrevEffect);
		m_pCurVertBuf = NULL;
	}

	if (m_pCurIndBuf)
	{
		m_pCurIndBuf->unbindFromPipeline();
		m_pCurIndBuf = NULL;
	}

	m_pCurOutput = NULL;

	m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);
}

void DrawList::do_RENDER_Z_ONLY(Events::Event *pEvt, int &threadOwnershipMask)
{
	do_RENDER(pEvt, threadOwnershipMask);
}

}; // namespace Components
}; // namespace PE
