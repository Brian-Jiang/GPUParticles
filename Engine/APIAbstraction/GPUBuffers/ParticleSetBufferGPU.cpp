#include "ParticleSetBufferGPU.h"

#include <cstdlib>

#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/Scene/DrawList.h"
#include "VertexBufferGPUManager.h"
#include "PrimeEngine/Utils/ErrorHandling.h"
#include "PrimeEngine/Scene/Mesh.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"
#include "PrimeEngine/Geometry/ParticleSystemCPU/ParticleSystemCPU.h"

using namespace PE;

#if PE_API_IS_D3D11
ID3D11ShaderResourceView* ParticleSetBufferGPU::s_pParticleCSMapResult1SRV = NULL;
ID3D11ShaderResourceView* ParticleSetBufferGPU::s_pParticleCSMapResult2SRV = NULL;
ID3D11UnorderedAccessView* ParticleSetBufferGPU::s_pParticleCSMapResult1UAV = NULL;
ID3D11UnorderedAccessView* ParticleSetBufferGPU::s_pParticleCSMapResult2UAV = NULL;
ID3D11Buffer* ParticleSetBufferGPU::s_pParticleCSResult1Buffer = NULL;
ID3D11Buffer* ParticleSetBufferGPU::s_pParticleCSResult2Buffer = NULL;

ID3D11Buffer* ParticleSetBufferGPU::s_pParticleEmitterBuffer = NULL;
ID3D11ShaderResourceView* ParticleSetBufferGPU::s_pParticleEmitterSRV = NULL;

ID3D11Buffer* ParticleSetBufferGPU::s_pParticleDrawArgsBuffer = NULL;
ID3D11UnorderedAccessView* ParticleSetBufferGPU::s_pParticleDrawArgsUAV = NULL;

ID3D11Buffer* ParticleSetBufferGPU::s_pParticleDispatchArgsBuffer = NULL;
ID3D11UnorderedAccessView* ParticleSetBufferGPU::s_pParticleDispatchArgsUAV = NULL;

bool ParticleSetBufferGPU::s_particleInitialized = false;
PE::Handle ParticleSetBufferGPU::s_hMesh = NULL;
//Components::MeshInstance* ParticleSetBufferGPU::s_pMeshInstance = NULL;

//void* ParticleSetBufferGPU::s_pParticleCSResultBufferCpuMirrorMem = NULL;
//int ParticleSetBufferGPU::s_pParticleCSResultBufferCpuMirrorMemSize = 0;

//ID3D11Buffer* ParticleSetBufferGPU::s_pParticleCSResultBuffersCpuStaging[PE_MAX_NUM_OF_BUFFER_STEPS];
#endif

void PE::ParticleSetBufferGPU::createGPUBufferForParticleCSResult(PE::GameContext& ctx) {
#if APIABSTRACTION_D3D9
#elif APIABSTRACTION_D3D11

	D3D11Renderer* pD3D11Renderer = static_cast<D3D11Renderer*>(ctx.getGPUScreen());
	ID3D11Device* pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext* pDeviceContext = pD3D11Renderer->m_pD3DContext;

	{
		typedef Particle ElementType;

		int numElements = PE_MAX_PARTICLE_COUNT_IN_DRAW_CALL;
		int byteSize = sizeof(ElementType) * numElements;

        // Create 2 buffers for compute shader to write to
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DEFAULT; // specify D3D11_USAGE_DEFAULT if not needed to access with cpu map()
		vbd.CPUAccessFlags = 0;
		vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; //D3D11_BIND_UNORDERED_ACCESS allows writing to resource and reading from resource at the same draw call
		vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbd.ByteWidth = byteSize;
		vbd.StructureByteStride = sizeof(ElementType);

        Particle* particles = new Particle[numElements];
        for (int i = 0; i < numElements; ++i) {
            particles[i].m_position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            particles[i].m_velocity = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            particles[i].m_acceleration = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            particles[i].m_age = -1.0f;
            particles[i].m_lifeTime = 0.0f;
            //particles[i].m_random = std::rand() / (float)RAND_MAX;
        }

		//void* ptr = malloc(byteSize);
		//memset(ptr, -1, byteSize);

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = particles;
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;

		HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &s_pParticleCSResult1Buffer);
		PEASSERT(SUCCEEDED(hr), "Error creating buffer 1");
		hr = pDevice->CreateBuffer(&vbd, &vinitData, &s_pParticleCSResult2Buffer);
		PEASSERT(SUCCEEDED(hr), "Error creating buffer 2");
		//free(ptr);

        delete[] particles;

		// create staging buffer for CPU read
		//{
		//	D3D11_BUFFER_DESC vbd;
		//	vbd.Usage = D3D11_USAGE_STAGING;
		//	vbd.BindFlags = 0;
		//	vbd.ByteWidth = byteSize;
		//	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  //          vbd.MiscFlags = 0;
		//	hr = pDevice->CreateBuffer(&vbd, NULL, &s_pParticleCSResultBufferCpuMirror);
		//	PEASSERT(SUCCEEDED(hr), "Error creating staging buffer");

		//	s_pParticleCSResultBufferCpuMirrorMem = malloc(byteSize);
		//	s_pParticleCSResultBufferCpuMirrorMemSize = byteSize;
		//}

		// animation compute shader Map stage result UAV (used by the Map) and View use by succeeding shaders
		// 2 view of the compute shader result buffer
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
			vdesc.Format = DXGI_FORMAT_UNKNOWN; // since using StructuredBuffer, has to be UNKNOWN
			vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			vdesc.Buffer.ElementOffset = 0;
			vdesc.Buffer.NumElements = numElements;

			hr = pDevice->CreateShaderResourceView(s_pParticleCSResult1Buffer, &vdesc, &s_pParticleCSMapResult1SRV);
			PEASSERT(SUCCEEDED(hr), "Error creating shader resource view 1");
			hr = pDevice->CreateShaderResourceView(s_pParticleCSResult2Buffer, &vdesc, &s_pParticleCSMapResult2SRV);
			PEASSERT(SUCCEEDED(hr), "Error creating shader resource view 2");

			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;  // since using StructuredBuffer, has to be UNKNOWN
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0; // could specify D3D11_BUFFER_UAV_FLAG_APPEND
			uavDesc.Buffer.NumElements = numElements;

			hr = pDevice->CreateUnorderedAccessView(s_pParticleCSResult1Buffer, &uavDesc, &s_pParticleCSMapResult1UAV);
			PEASSERT(SUCCEEDED(hr), "Error creating UAV 1");
			hr = pDevice->CreateUnorderedAccessView(s_pParticleCSResult2Buffer, &uavDesc, &s_pParticleCSMapResult2UAV);
			PEASSERT(SUCCEEDED(hr), "Error creating UAV 2");
		}

		// create staging buffers for each step of the animation compute shader
		//for (int i = 0; i < PE_MAX_NUM_OF_BUFFER_STEPS; ++i) {
		//	D3D11_BUFFER_DESC vbd;
		//	vbd.Usage = D3D11_USAGE_STAGING;
		//	vbd.BindFlags = 0;
		//	vbd.ByteWidth = byteSize / PE_MAX_NUM_OF_BUFFER_STEPS;
		//	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		//	vbd.MiscFlags = 0;
		//	hr = pDevice->CreateBuffer(&vbd, NULL, &s_pParticleCSResultBuffersCpuStaging[i]);
		//	PEASSERT(SUCCEEDED(hr), "Error creating staging buffer");
		//}
	}

	// create emitter resource
	{
        // only one emitter for now
        int byteSize = sizeof(ParticleEmitter);

		// Create the buffer itself
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC; // specify D3D11_USAGE_DEFAULT if not needed to access with cpu map()
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbd.ByteWidth = byteSize;
        vbd.StructureByteStride = sizeof(ParticleEmitter);

		ParticleEmitter* emitter = new ParticleEmitter();
        emitter->m_position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        emitter->emissionRate = 3500.0f;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = emitter;
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;

		HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &s_pParticleEmitterBuffer);
		PEASSERT(SUCCEEDED(hr), "Error creating emitter buffer");
		delete emitter;

        // create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
		vdesc.Format = DXGI_FORMAT_UNKNOWN;
		vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		vdesc.Buffer.ElementOffset = 0;
		vdesc.Buffer.NumElements = 1;

		hr = pDevice->CreateShaderResourceView(s_pParticleEmitterBuffer, &vdesc, &s_pParticleEmitterSRV);
		PEASSERT(SUCCEEDED(hr), "Error creating emitter shader resource view");
	}

	// create dispatch args
	{
		// Describe the buffer
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;  // Default usage (GPU read/write)
		bufferDesc.ByteWidth = sizeof(D3D11_DISPATCH_INDIRECT_ARGS);  // Size of the buffer in bytes
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;  // Bind as UAV
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

        D3D11_DISPATCH_INDIRECT_ARGS dispatchArgs = {};
        dispatchArgs.ThreadGroupCountX = 0;
        dispatchArgs.ThreadGroupCountY = 1;
        dispatchArgs.ThreadGroupCountZ = 1;

		// Define initial data for the buffer
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &dispatchArgs;
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;

		HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &vinitData, &s_pParticleDispatchArgsBuffer);
		PEASSERT(SUCCEEDED(hr), "Error creating dispatch args buffer");

		// Describe the UAV for the buffer
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = sizeof(D3D11_DISPATCH_INDIRECT_ARGS) / sizeof(UINT);
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		hr = pDevice->CreateUnorderedAccessView(s_pParticleDispatchArgsBuffer, &uavDesc, &s_pParticleDispatchArgsUAV);
		PEASSERT(SUCCEEDED(hr), "Error creating dispatch args uav");
	}

#endif
}

void PE::ParticleSetBufferGPU::initializeParticleMeshInstance(PE::GameContext& ctx, PE::Handle hMesh) {
	if (s_particleInitialized) {
		return;
	}

	s_particleInitialized = true;

	D3D11Renderer* pD3D11Renderer = static_cast<D3D11Renderer*>(ctx.getGPUScreen());
	ID3D11Device* pDevice = pD3D11Renderer->m_pD3DDevice;

	//Handle h = ctx.getMeshManager()->getAsset("imrod.x_imrodmesh_mesh.mesha", "Default", ctx.m_gameThreadThreadOwnershipMask);
    s_hMesh = hMesh;
	Components::Mesh* mesh = hMesh.getObject<Components::Mesh>();
	IndexBufferGPU* pIB = mesh->m_hIndexBufferGPU.getObject<IndexBufferGPU>();
	UINT32 length = pIB->m_length;

	// Describe the buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;  // Default usage (GPU read/write)
	bufferDesc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);  // Size of the buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_VERTEX_BUFFER;  // Bind as UAV and vertex buffer
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	// Define and initialize draw arguments (zeroed initially)
	D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS drawArgs = {};
	drawArgs.IndexCountPerInstance = length;
	drawArgs.InstanceCount = 0;  // Will be updated by compute shader
	drawArgs.StartIndexLocation = 0;
	drawArgs.BaseVertexLocation = 0;
	drawArgs.StartInstanceLocation = 0;

	// Define initial data for the buffer
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &drawArgs;
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;

	HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &vinitData, &s_pParticleDrawArgsBuffer);
	PEASSERT(SUCCEEDED(hr), "Error creating draw args buffer");

	// Describe the UAV for the buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS) / sizeof(UINT);
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

	hr = pDevice->CreateUnorderedAccessView(s_pParticleDrawArgsBuffer, &uavDesc, &s_pParticleDrawArgsUAV);
	PEASSERT(SUCCEEDED(hr), "Error creating draw args uav");
}

void ParticleSetBufferGPU::bindUAV1(GameContext& context, MemoryArena arena, Components::DrawList* pDrawList) {
#if PE_API_IS_D3D11
	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pParticleCSMapResult1UAV != NULL, "shader uav 1 not set");
	// no need to copy back
	pSetTextureAction->set(
        GpuResourceSlot_ParticleCS1UAV,  // u1
		s_pParticleCSMapResult1UAV,
		nullptr,
		nullptr,
		0,
		"s_pParticleCSMapResult1UAV"
	);
#endif
}

void ParticleSetBufferGPU::bindUAV2(GameContext& context, MemoryArena arena, Components::DrawList* pDrawList) {
#if PE_API_IS_D3D11
	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pParticleCSMapResult2UAV != NULL, "shader uav 2 not set");
	// no need to copy back
	pSetTextureAction->set(
        GpuResourceSlot_ParticleCS2UAV,  // u2
		s_pParticleCSMapResult2UAV,
		nullptr,
		nullptr,
		0,
		"s_pParticleCSMapResult2UAV"
	);
#endif
}

void PE::ParticleSetBufferGPU::bindSRV1(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList) {
	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pParticleCSMapResult1SRV != NULL, "shader srv 1 not set");
	pSetTextureAction->set(
		GpuResourceSlot_ParticleCSResult1Resource,  // t71
		SamplerState_NotNeeded,
		s_pParticleCSMapResult1SRV,
		"s_pParticleCSMapResult1SRV"
	);
}

void PE::ParticleSetBufferGPU::bindSRV2(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList) {
    Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
    SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

    PEASSERT(s_pParticleCSMapResult2SRV != NULL, "shader srv 2 not set");
    pSetTextureAction->set(
        GpuResourceSlot_ParticleCSResult2Resource,  // t72
        SamplerState_NotNeeded,
        s_pParticleCSMapResult2SRV,
        "s_pParticleCSMapResult2SRV"
    );
}

void PE::ParticleSetBufferGPU::bindDrawArgsUAV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList) {
#if PE_API_IS_D3D11
	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pParticleDrawArgsUAV != NULL, "draw args uav not set");
	// no need to copy back
	pSetTextureAction->set(
		GpuResourceSlot_ParticleDrawArgsUAV,  // u3
		s_pParticleDrawArgsUAV,
		nullptr,
		nullptr,
		0,
		"s_pParticleDrawArgsUAV"
	);
#endif
}

void PE::ParticleSetBufferGPU::bindDispatchArgsUAV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList) {
#if PE_API_IS_D3D11
    Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
    SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

    PEASSERT(s_pParticleDispatchArgsUAV != NULL, "dispatch args uav not set");
    // no need to copy back
    pSetTextureAction->set(
        GpuResourceSlot_ParticleDispatchArgsUAV,  // u4
        s_pParticleDispatchArgsUAV,
        nullptr,
        nullptr,
        0,
        "s_pParticleDispatchArgsUAV"
    );
#endif
}

void PE::ParticleSetBufferGPU::bindEmitterSRV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList) {
#if PE_API_IS_D3D11
	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);

	PEASSERT(s_pParticleEmitterSRV != NULL, "emitter srv not set");
	pSetTextureAction->set(
		GpuResourceSlot_ParticleEmitterResource,  // t70
		SamplerState_NotNeeded,
		s_pParticleEmitterSRV,
		"s_pParticleEmitterSRV"
	);
#endif
}

void PE::ParticleSetBufferGPU::updateEmitterBuffer(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList, const Vector4& position, float emissionRate) {
	D3D11Renderer* pD3D11Renderer = static_cast<D3D11Renderer*>(context.getGPUScreen());
	ID3D11Device* pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext* pDeviceContext = pD3D11Renderer->m_pD3DContext;

	// Map the buffer to CPU memory
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11Resource* srvBufferResource;
	s_pParticleEmitterSRV->GetResource(&srvBufferResource);
	HRESULT hr = pDeviceContext->Map(srvBufferResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	PEASSERT(SUCCEEDED(hr), "Error mapping emitter srv");

	ParticleEmitter* emitter = new ParticleEmitter();
	emitter->m_position = position;
	emitter->emissionRate = emissionRate;

	// Copy the new data into the buffer
	memcpy(mappedResource.pData, emitter, sizeof(ParticleEmitter));

	// Unmap the buffer to apply changes
	pDeviceContext->Unmap(srvBufferResource, 0);

    delete emitter;
}

//void ParticleSetBufferGPU::createShaderValueForVSViewOfCSMap(GameContext& context, MemoryArena arena, Components::DrawList* pDrawList) {
//#if PE_API_IS_D3D11
//	Handle& hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
//	SA_Bind_Resource* pSetTextureAction = new(hSV) SA_Bind_Resource(context, arena);
//
//	PEASSERT(s_pParticleCSMapResult1UAV != NULL, "shader uav not set");
//	//pSetTextureAction->set(
//	//	GpuResourceSlot_ParticleCSResultResource,
//	//	SamplerState_NotNeeded,
//	//	s_pParticleCSMapResultShaderResourceView,
//	//	"s_pParticleCSMapResultShaderResourceView");
//#endif
//}
