#ifndef __PYENGINE_2_0_SKELETON_H__
#define __PYENGINE_2_0_SKELETON_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes

#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {
struct DrawList;
struct Skeleton : public Component
{
	PE_DECLARE_CLASS(Skeleton);

	Skeleton(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	void initFromFiles(const char *skeletonAssetName, const char *skeletonAssetPackage,
		int &threadOwnershipMask);

	void addSA_SkeletonStrucuture(PE::Components::DrawList *pDrawList);

	void addSA_SkeletonBindInverses(PE::Components::DrawList *pDrawList);

	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_GetSkeleton(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

	//todo: these should probably go into SkeletonAsset, since we don't want to duplicate these variables in each instance
	Handle m_hSkeletonCPU;
	PrimitiveTypes::Float32 m_skinScaleFactor;
#if PE_API_IS_D3D11
	ID3D11Buffer *m_pSkeletonStructureBuffer;
	ID3D11ShaderResourceView *m_pSkeletonStructureView;

	ID3D11Buffer *m_pSkeletonBindInversesBuffer;
	ID3D11ShaderResourceView *m_pSkeletonBindInversesView;
#endif
};
}; // namespace Components
}; // namespace PE

#endif
