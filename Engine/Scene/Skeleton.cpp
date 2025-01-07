#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetCPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Render/ShaderActions/SetPerObjectConstantsShaderAction.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetGPUManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"

// Sibling/Children includes
#include "Skeleton.h"
#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "DefaultAnimationSM.h"
#include "MeshInstance.h"
#include "SkeletonInstance.h"
using namespace PE::Events;

namespace PE {
namespace Components {


PE_IMPLEMENT_CLASS1(Skeleton, Component);

Skeleton::Skeleton(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	, m_skinScaleFactor(1.0f)
{

}

void Skeleton::initFromFiles(const char *skeletonAssetName, const char *skeletonAssetPackage,
	int &threadOwnershipMask)
{
	PE::Handle hSkel("SkeletonCPU", sizeof(SkeletonCPU));
	SkeletonCPU *pSkel = new(hSkel) SkeletonCPU(*m_pContext, m_arena);
	pSkel->ReadSkeleton(skeletonAssetName, skeletonAssetPackage);

	m_hSkeletonCPU = hSkel;
	
#if APIABSTRACTION_D3D9
#elif APIABSTRACTION_D3D11

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	int numElements = PE_MAX_BONE_COUNT_IN_DRAW_CALL;
	int byteSize = sizeof(JointParentData) * numElements;

	// Create the buffer itself
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT; // specify D3D11_USAGE_DEFAULT if not needed to access with cpu map()
	vbd.CPUAccessFlags = 0;
	vbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	vbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vbd.ByteWidth =  byteSize;
	vbd.StructureByteStride = sizeof(JointParentData);


	PEASSERT(byteSize >= pSkel->m_jointParents.m_size * sizeof(JointParentData), "Wrong array size");

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pSkel->m_jointParents.getFirstPtr();
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;


	ID3D11Buffer *pBuffer;

	HRESULT hr = pDevice->CreateBuffer(&vbd, &vinitData, &pBuffer);



	PEASSERT(SUCCEEDED(hr), "Error creating buffer");
	m_pSkeletonStructureBuffer = pBuffer;

	D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
	vdesc.Format = DXGI_FORMAT_UNKNOWN; // since using StructuredBuffer, has to be UNKNOWN
	vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	vdesc.Buffer.ElementOffset = 0;
	vdesc.Buffer.NumElements = numElements;

	ID3D11ShaderResourceView *pShaderResourceView = NULL;

	hr = pDevice->CreateShaderResourceView(pBuffer, &vdesc, &pShaderResourceView);
	PEASSERT(SUCCEEDED(hr), "Error creating shader resource view");
	m_pSkeletonStructureView = pShaderResourceView;

	// bind inverses. can reuse the data structs

	byteSize = sizeof(Matrix4x4) * numElements;

	// Create the buffer itself
	vbd.ByteWidth =  byteSize;
	vbd.StructureByteStride = sizeof(Matrix4x4);

	SkeletonCPU &skel = *m_hSkeletonCPU.getObject<SkeletonCPU>();
	PEASSERT(byteSize >= skel.m_numJoints * sizeof(Matrix4x4), "Worng array size");

	vinitData.pSysMem = skel.getBindInversesPtr();

	hr = pDevice->CreateBuffer(&vbd, &vinitData, &pBuffer);

	PEASSERT(SUCCEEDED(hr), "Error creating buffer");
	m_pSkeletonBindInversesBuffer = pBuffer;

	hr = pDevice->CreateShaderResourceView(pBuffer, &vdesc, &pShaderResourceView);
	PEASSERT(SUCCEEDED(hr), "Error creating shader resource view");
	m_pSkeletonBindInversesView = pShaderResourceView;
#endif
}

void Skeleton::addSA_SkeletonStrucuture(PE::Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(*m_pContext, m_arena);

	PEASSERT(m_pSkeletonStructureView != NULL, "shader resource not set");
	pSetTextureAction->set(
		GpuResourceSlot_SkeletonStructure_ConstResource,
		SamplerState_NotNeeded,
		m_pSkeletonStructureView,
		m_hSkeletonCPU.getObject<SkeletonCPU>()->m_name
		);
#endif
}

void Skeleton::addSA_SkeletonBindInverses(PE::Components::DrawList *pDrawList)
{
#if PE_API_IS_D3D11
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
	SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(*m_pContext, m_arena);

	PEASSERT(m_pSkeletonBindInversesView != NULL, "shader resource not set");

	pSetTextureAction->set(
		GpuResourceSlot_SkeletonBindInverses_ConstResource,
		SamplerState_NotNeeded,
		m_pSkeletonBindInversesView,
		m_hSkeletonCPU.getObject<SkeletonCPU>()->m_name
		);
#endif
}


//////////////////////////////////////////////////////////////////////////
// SkeletonInstance Lua Interface
//////////////////////////////////////////////////////////////////////////
//
// todo: these functions might be better off in Skeleton asset whenever that is implemented
void Skeleton::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Skin[] = {
		{"l_GetSkeleton", l_GetSkeleton},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_Skin);

	// run a script to add additional functionality to Lua side of Skin
	// that is accessible from Lua
	pLuaEnv->runScript(PEString::generateScriptPathname(*pLuaEnv->getGameContext(), "Skeleton.lua", "PrimeEngine", "Scene", PEString::s_buf, PEString::BUF_SIZE));
}
//
// retrieves debug information of the skin
int Skeleton::l_GetSkeleton(lua_State *luaVM)
{
	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	SkeletonInstance *pSkelInstance = h.getObject<SkeletonInstance>();

	Skeleton *pSkeleton = pSkelInstance->getFirstParentByTypePtr<Skeleton>();

	// table that will be returned
	lua_newtable(luaVM);

	lua_pushstring(luaVM, "anims"); // 
	lua_newtable(luaVM); // list of names
	{
		int iAnim = 0;
		for (int iAnimSet = 0; iAnimSet < pSkelInstance->m_hAnimationSetGPUs.m_size; ++iAnimSet)
		{
			AnimSetBufferGPU *pAnimSetGPU = pSkelInstance->m_hAnimationSetGPUs[iAnimSet].getObject<AnimSetBufferGPU>();
			AnimationSetCPU *pAnims = pAnimSetGPU->m_hAnimationSetCPU.getObject<AnimationSetCPU>();
			for (int i = 0; i < (int)(pAnims->m_animations.m_size); i++)
			{
				AnimationCPU &animCpu = pAnims->m_animations[i];
				lua_pushnumber(luaVM, iAnim+1); // push index of the name
				
				{
					lua_newtable(luaVM);
					lua_pushstring(luaVM, "name");
					lua_pushstring(luaVM, animCpu.m_name);
					lua_rawset(luaVM, -3);
					lua_pushstring(luaVM, "animSetIndex");
					lua_pushnumber(luaVM, iAnimSet);
					lua_rawset(luaVM, -3);
					lua_pushstring(luaVM, "index");
					lua_pushnumber(luaVM, i);
					lua_rawset(luaVM, -3);
				}
				
				lua_rawset(luaVM, -3); // set result["animNames"][i+1]

				++iAnim;
			}
		}
	}
	lua_rawset(luaVM, -3); // set list of names

	return 1; // the result is the table on the stack
}
//////////////////////////////////////////////////////////////////////////

	
}; // namespace Components
}; // namespace PE

