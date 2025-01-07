#pragma once

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "PrimeEngine/Geometry/TangentBufferCPU/TangentBufferCPU.h"
#include "PrimeEngine/Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "PrimeEngine/Geometry/ParticleSystemCPU/ParticleBufferCPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/MeshManager.h"

#include "PrimeEngine/Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "PrimeEngine/Geometry/ColorBufferCPU.h"
#if APIABSTRACTION_D3D9
#include "PrimeEngine/APIAbstraction/DirectX9/D3D9_GPUBuffers/D3D9_VertexBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "PrimeEngine/APIAbstraction/DirectX11/D3D11_GPUBuffers/D3D11_VertexBufferGPU.h"
#elif APIABSTRACTION_OGL
#include "PrimeEngine/APIAbstraction/OGL/OGL_GPUBuffers/OGL_VertexBufferGPU.h"
#endif

#include "IndexBufferGPU.h"
#include "BufferInfo.h"

// Sibling/Children includes
namespace PE {
    namespace Components {
        struct Effect;
        struct DrawList;
    }

    struct D3D11_DISPATCH_INDIRECT_ARGS {
        UINT ThreadGroupCountX;
        UINT ThreadGroupCountY;
        UINT ThreadGroupCountZ;
    };

    class ParticleSetBufferGPU : public PE::PEAllocatableAndDefragmentable {

    public:
        static void createGPUBufferForParticleCSResult(PE::GameContext& ctx);
        static void initializeParticleMeshInstance(PE::GameContext& ctx, PE::Handle hMesh);
        static void bindUAV1(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindUAV2(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindSRV1(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindSRV2(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindDrawArgsUAV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindDispatchArgsUAV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void bindEmitterSRV(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList);
        static void updateEmitterBuffer(PE::GameContext& context, PE::MemoryArena arena, Components::DrawList* pDrawList, const Vector4& position, float emissionRate);

        static ID3D11Buffer* getDrawArgsBuffer() { return s_pParticleDrawArgsBuffer; }
        static ID3D11Buffer* getDispatchArgsBuffer() { return s_pParticleDispatchArgsBuffer; }

        static bool s_particleInitialized;
        static PE::Handle s_hMesh;

    private:
        static ID3D11ShaderResourceView* s_pParticleCSMapResult1SRV;
        static ID3D11UnorderedAccessView* s_pParticleCSMapResult1UAV;
        static ID3D11Buffer* s_pParticleCSResult1Buffer;

        static ID3D11ShaderResourceView* s_pParticleCSMapResult2SRV;
        static ID3D11UnorderedAccessView* s_pParticleCSMapResult2UAV;
        static ID3D11Buffer* s_pParticleCSResult2Buffer;
        //static ID3D11Buffer* s_pParticleCSResultBufferCpuMirror;
        //static void* s_pParticleCSResultBufferCpuMirrorMem;
        //static int s_pParticleCSResultBufferCpuMirrorMemSize;

        //static ID3D11Buffer* s_pParticleCSResultBuffersCpuStaging[PE_MAX_NUM_OF_BUFFER_STEPS];

        static ID3D11Buffer* s_pParticleEmitterBuffer;
        static ID3D11ShaderResourceView* s_pParticleEmitterSRV;

        static ID3D11Buffer* s_pParticleDrawArgsBuffer;
        static ID3D11UnorderedAccessView* s_pParticleDrawArgsUAV;
        //static ID3D11ShaderResourceView* s_pParticleByteAddressResourceView;

        static ID3D11Buffer* s_pParticleDispatchArgsBuffer;
        static ID3D11UnorderedAccessView* s_pParticleDispatchArgsUAV;
        
        //static Components::MeshInstance* s_pMeshInstance;
    };
}

