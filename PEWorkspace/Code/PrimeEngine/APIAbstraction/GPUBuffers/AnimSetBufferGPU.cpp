
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "AnimSetBufferGPU.h"

#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/Scene/DrawList.h"
#include "VertexBufferGPUManager.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"
namespace PE {

#if PE_API_IS_D3D11
ID3D11ShaderResourceView *AnimSetBufferGPU::s_pAnimationCSMapResultShaderResourceView = NULL;
ID3D11UnorderedAccessView *AnimSetBufferGPU::s_pAnimationCSMapResultShaderUAView = NULL;
ID3D11Buffer *AnimSetBufferGPU::s_pAnimationCSResultBuffer = NULL;
ID3D11Buffer *AnimSetBufferGPU::s_pAnimationCSResultBufferCpuMirror = NULL;
void *AnimSetBufferGPU::s_pAnimationCSResultBufferCpuMirrorMem = NULL;
int AnimSetBufferGPU::s_pAnimationCSResultBufferCpuMirrorMemSize = 0;


ID3D11UnorderedAccessView *AnimSetBufferGPU::s_pCSReduceComputeTargetUAV = NULL;
ID3D11ShaderResourceView *AnimSetBufferGPU::s_pCSReduceComputeTargetView = NULL;
ID3D11Buffer *AnimSetBufferGPU::s_pCSReduceComputeTargetBuffer = NULL;
ID3D11Buffer *AnimSetBufferGPU::s_pCSReduceComputeTargetStagingBuffer = NULL;
void *AnimSetBufferGPU::s_pCSReduceComputeTargetCpuMem = NULL;
int AnimSetBufferGPU::s_pCSReduceComputeTargetCpuMemSize = NULL;


ID3D11Buffer *AnimSetBufferGPU::s_pAnimationCSResultBuffersCpuStaging[PE_MAX_NUM_OF_BUFFER_STEPS];
	
#endif

AnimSetBufferGPU::AnimSetBufferGPU(PE::GameContext &context, PE::MemoryArena arena)
{
	m_arena = arena; m_pContext = &context;
}

AnimSetBufferGPU::~AnimSetBufferGPU()
{
	releaseGPUBuffer();
}

void AnimSetBufferGPU::createGPUBufferFromAnimSet(AnimationSetCPU &animSetCpu)
{
	#if APIABSTRACTION_D3D9
	#elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		int numElements = PE_MAX_BONE_COUNT_IN_DRAW_CALL * PE_MAX_FRAMES_IN_ANIMATION * PE_MAX_ANIMATIONS_IN_BUFFER;
		int byteSize = sizeof(BoneTQ) * numElements;

		Array<BoneTQ> data(*m_pContext, m_arena);
		data.reset(numElements);
		data.m_size = numElements;

		int numAnims = animSetCpu.m_animations.m_size;

		if (numAnims > PE_MAX_ANIMATIONS_IN_BUFFER)
		{
			PEWARN("Will not fit all anims in AnimSetBuffer");
			numAnims = PE_MAX_ANIMATIONS_IN_BUFFER;
		}

		for (int iAnim = 0; iAnim < numAnims; ++iAnim)
		{
			BoneTQ *pAnimData = &data[PE_MAX_BONE_COUNT_IN_DRAW_CALL * PE_MAX_FRAMES_IN_ANIMATION * iAnim];

			AnimationCPU &anim = animSetCpu.m_animations[iAnim];
			PEASSERT(anim.m_numJoints <= PE_MAX_BONE_COUNT_IN_DRAW_CALL, "Too big skeleton!");
			int numFrames = anim.m_frames.m_size;
			if (numFrames > PE_MAX_FRAMES_IN_ANIMATION)
			{
				PEWARN("Will not fit all animation frames in animation");
				numFrames = PE_MAX_FRAMES_IN_ANIMATION;
			}
			for (int iFrame = 0; iFrame < numFrames; ++iFrame)
			{
				BoneTQ *pFrameData = pAnimData + (PE_MAX_BONE_COUNT_IN_DRAW_CALL * iFrame);
				Array<TSQ> &frame = anim.m_frames[iFrame];
				for (int iBone = 0; iBone < anim.m_numJoints; ++iBone, ++pFrameData)
				{
					pFrameData->m_quat = frame[iBone].m_quat;
					pFrameData->m_translation = frame[iBone].m_translation;
				}
			}
			for (int iFrame = numFrames; iFrame < PE_MAX_FRAMES_IN_ANIMATION; ++iFrame)
			{
				BoneTQ *pFrameData = pAnimData + (PE_MAX_BONE_COUNT_IN_DRAW_CALL * iFrame);
				Array<TSQ> &frame = anim.m_frames[numFrames-1];
				for (int iBone = 0; iBone < anim.m_numJoints; ++iBone, ++pFrameData)
				{
					pFrameData->m_quat = frame[iBone].m_quat;
					pFrameData->m_translation = frame[iBone].m_translation;
				}
			}
		}


		// Create the buffer itself
		D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DEFAULT;
			vbd.CPUAccessFlags = 0;
			vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE/* | D3D11_BIND_UNORDERED_ACCESS*/;
			vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			vbd.ByteWidth =  byteSize;
			vbd.StructureByteStride = sizeof(BoneTQ);


		ID3D11Buffer *pBuffer;
		
		D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = data.getFirstPtr();
			vinitData.SysMemPitch = 0;
			vinitData.SysMemSlicePitch = 0;
		HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &pBuffer);
		PEASSERT(SUCCEEDED(hr), "Error creating buffer");
		m_pBuf = pBuffer;


		D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
			vdesc.Format = DXGI_FORMAT_UNKNOWN;
			vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			vdesc.Buffer.ElementOffset = 0;
			vdesc.Buffer.NumElements = numElements;

		ID3D11ShaderResourceView *pShaderResourceView = NULL;


		hr = pDevice->CreateShaderResourceView(pBuffer, &vdesc, &pShaderResourceView);
		PEASSERT(SUCCEEDED(hr), "Error creating shader resource view");
		m_pShaderResourceView = pShaderResourceView;
		/*
		if(useAsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = appendConsume ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
		uavDesc.Buffer.NumElements = numElements;
		DXCall(device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView));
	}
	*/
	#endif
}


void AnimSetBufferGPU::createGPUBufferForAnimationCSResult(PE::GameContext &ctx)
{
	#if APIABSTRACTION_D3D9
	#elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(ctx.getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
	
		{
			#if PE_STORE_CS_MAP_RESULT_AS_MATRIX
				typedef Matrix4x4 ElementType;
			#else
				typedef BoneTQ ElementType;
			#endif

			int numElements = PE_MAX_BONE_COUNT_IN_DRAW_CALL * PE_MAX_SKINED_INSTANCE_COUNT_IN_COMPUTE_CALL;
			int byteSize = sizeof(ElementType) * numElements;

			// Create the buffer itself
			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DEFAULT; // specify D3D11_USAGE_DEFAULT if not needed to access with cpu map()
			vbd.CPUAccessFlags = 0;
			vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; //D3D11_BIND_UNORDERED_ACCESS allows writing to resource and reading from resource at the same draw call
			vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			vbd.ByteWidth =  byteSize;
			vbd.StructureByteStride = sizeof(ElementType);

			void *ptr = malloc(byteSize);
			memset(ptr, -1, byteSize);

			D3D11_SUBRESOURCE_DATA vinitData;
				vinitData.pSysMem = ptr;
				vinitData.SysMemPitch = 0;
				vinitData.SysMemSlicePitch = 0;
		
			HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &s_pAnimationCSResultBuffer);
			PEASSERT(SUCCEEDED(hr), "Error creating buffer");
			free(ptr);

            // create staging buffer for CPU read
			{
				vbd.Usage = D3D11_USAGE_STAGING;
				vbd.BindFlags = 0;
				vbd.ByteWidth =  byteSize;
				vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				hr = pDevice->CreateBuffer(&vbd, NULL, &s_pAnimationCSResultBufferCpuMirror);
				PEASSERT(SUCCEEDED(hr), "Error creating staging buffer");

				s_pAnimationCSResultBufferCpuMirrorMem = malloc(byteSize);
				s_pAnimationCSResultBufferCpuMirrorMemSize = byteSize;
			}

			// animation compute shader Map stage result UAV (used by the Map) and View use by succeeding shaders
            // 2 view of the compute shader result buffer
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
				vdesc.Format = DXGI_FORMAT_UNKNOWN; // since using StructuredBuffer, has to be UNKNOWN
				vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				vdesc.Buffer.ElementOffset = 0;
				vdesc.Buffer.NumElements = numElements;

				hr = pDevice->CreateShaderResourceView(s_pAnimationCSResultBuffer, &vdesc, &s_pAnimationCSMapResultShaderResourceView);
				PEASSERT(SUCCEEDED(hr), "Error creating shader resource view");

				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.Flags = 0; // could specify D3D11_BUFFER_UAV_FLAG_APPEND
				uavDesc.Buffer.NumElements = numElements;

				hr = pDevice->CreateUnorderedAccessView(s_pAnimationCSResultBuffer, &uavDesc, &s_pAnimationCSMapResultShaderUAView);
				PEASSERT(SUCCEEDED(hr), "Error creating UAV");
			}

            // create staging buffers for each step of the animation compute shader
			for (int i = 0; i < PE_MAX_NUM_OF_BUFFER_STEPS; ++i)
			{
				vbd.Usage = D3D11_USAGE_STAGING;
				vbd.BindFlags = 0;
				vbd.ByteWidth =  byteSize / PE_MAX_NUM_OF_BUFFER_STEPS;
				vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				hr = pDevice->CreateBuffer(&vbd, NULL, &s_pAnimationCSResultBuffersCpuStaging[i]);
				PEASSERT(SUCCEEDED(hr), "Error creating staging buffer");
			}
		}


		{
			typedef Matrix4x4 ElementType;

			int numElements = PE_MAX_BONE_COUNT_IN_DRAW_CALL * PE_MAX_SKINED_INSTANCE_COUNT_IN_COMPUTE_CALL;
			int byteSize = sizeof(ElementType) * numElements;

			// Create the buffer itself
			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DEFAULT; // specify D3D11_USAGE_DEFAULT if not needed to access with cpu map()
			vbd.CPUAccessFlags = 0;
			vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; //D3D11_BIND_UNORDERED_ACCESS allows writing to resource and reading from resource at the same draw call
			vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			vbd.ByteWidth =  byteSize;
			vbd.StructureByteStride = sizeof(ElementType);

			void *ptr = malloc(byteSize);
			memset(ptr, -1, byteSize);

			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = ptr;
			vinitData.SysMemPitch = 0;
			vinitData.SysMemSlicePitch = 0;

			HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &s_pCSReduceComputeTargetBuffer);
			PEASSERT(SUCCEEDED(hr), "Error creating buffer");
			free(ptr);

			{
				vbd.Usage = D3D11_USAGE_STAGING;
				vbd.BindFlags = 0;
				vbd.ByteWidth =  byteSize;
				vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				hr = pDevice->CreateBuffer(&vbd, NULL, &s_pCSReduceComputeTargetStagingBuffer);
				PEASSERT(SUCCEEDED(hr), "Error creating staging buffer");

				s_pCSReduceComputeTargetCpuMem = malloc(byteSize);
				s_pCSReduceComputeTargetCpuMemSize = byteSize;
			}

			// animation compute shader Map stage result UAV (used by the Map) and View use by succeeding shaders
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
				vdesc.Format = DXGI_FORMAT_UNKNOWN; // since using StructuredBuffer, has to be UNKNOWN
				vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				vdesc.Buffer.ElementOffset = 0;
				vdesc.Buffer.NumElements = numElements;

				hr = pDevice->CreateShaderResourceView(s_pCSReduceComputeTargetBuffer, &vdesc, &s_pCSReduceComputeTargetView);
				PEASSERT(SUCCEEDED(hr), "Error creating shader resource view");

				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.Flags = 0; // could specify D3D11_BUFFER_UAV_FLAG_APPEND
				uavDesc.Buffer.NumElements = numElements;

				hr = pDevice->CreateUnorderedAccessView(s_pCSReduceComputeTargetBuffer, &uavDesc, &s_pCSReduceComputeTargetUAV);
				PEASSERT(SUCCEEDED(hr), "Error creating UAV");
			}
		}
	#endif
}

void AnimSetBufferGPU::createShaderValue(Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(*m_pContext, m_arena);


	PEASSERT(m_pShaderResourceView != NULL, "shader resource not set");
	pSetTextureAction->set(
		GpuResourceSlot_AnimSetData_ConstResource,  // t66
		SamplerState_NotNeeded,
		m_pShaderResourceView,
		m_hAnimationSetCPU.getObject<AnimationSetCPU>()->m_name
		);
#endif
}


void AnimSetBufferGPU::createShaderValueForCSMapUAV(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pAnimationCSMapResultShaderUAView != NULL, "shader uav not set");
	pSetTextureAction->set(
		GpuResourceSlot_AnimationCSUAV,  // u0
		s_pAnimationCSMapResultShaderUAView,
		s_pAnimationCSResultBufferCpuMirror,
		s_pAnimationCSResultBufferCpuMirrorMem,
		s_pAnimationCSResultBufferCpuMirrorMemSize,
		"s_pAnimationCSMapResultShaderUAView"
		);
#endif
}

void AnimSetBufferGPU::createShaderValueForVSViewOfCSMap(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pAnimationCSMapResultShaderResourceView != NULL, "shader uav not set");
	pSetTextureAction->set(
		GpuResourceSlot_AnimationCSResultResource,  // t67
		SamplerState_NotNeeded,
		s_pAnimationCSMapResultShaderResourceView,
		"s_pAnimationCSMapResultShaderResourceView");
#endif
}

void AnimSetBufferGPU::createShaderValueForCSReduceUAV(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pAnimationCSMapResultShaderUAView != NULL, "shader uav not set");
	pSetTextureAction->set(
		GpuResourceSlot_AnimationCSReduceUAV,  // u0
		s_pCSReduceComputeTargetUAV,
		s_pCSReduceComputeTargetStagingBuffer,
		s_pCSReduceComputeTargetCpuMem,
		s_pCSReduceComputeTargetCpuMemSize,
		"s_pCSReduceComputeTargetUAV"
		);
#endif
}

void AnimSetBufferGPU::createShaderValueForVSViewOfCSReduce(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pAnimationCSMapResultShaderResourceView != NULL, "shader uav not set");
	pSetTextureAction->set(
		GpuResourceSlot_AnimationCSReduceResultResource,
		SamplerState_NotNeeded,
		s_pCSReduceComputeTargetView,
		"s_pCSReduceComputeTargetView");
#endif
}

void AnimSetBufferGPU::mapCSResultsForCpuRead(PE::GameContext &context, PE::MemoryArena arena)
{

}

void AnimSetBufferGPU::releaseGPUBuffer()
{
	#if !PE_API_IS_D3D11
		PEINFO("Not implemented!");
	#endif
}

}; // namespace PE
