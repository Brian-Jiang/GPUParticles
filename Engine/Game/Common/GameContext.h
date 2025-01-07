
#ifndef __PrimeEngine_GameContext_h__
#define __PrimeEngine_GameContext_h__

#include "PrimeEngine/MemoryManagement/MemoryPool.h"

struct MainFunctionArgs;

namespace PE {
namespace Components{
	struct Component;
	struct LuaEnvironment;
	class Log;
	struct NetworkManager;
	struct GameObjectManager;
	struct DefaultGameControls;
	struct MeshManager;
    
};
class Application;
class IRenderer;
struct PERasterizerStateManager;
struct PEAlphaBlendStateManager;
struct PEDepthStencilStateManager;
struct GameContext
{
	Components::Component *getGame() { return m_pGame; }
	Components::LuaEnvironment *getLuaEnvironment(){return m_pLuaEnv;}
	MainFunctionArgs *getMainFunctionArgs(){return m_pMPArgs;}
	Application *getApplication(){return m_pApplication;}
	IRenderer *getGPUScreen(){return m_pGPUScreen;}
	PERasterizerStateManager *getRasterizerStateManager(){return m_pRaterizerStateManager;}
    PEAlphaBlendStateManager *getAlphaBlendStateManager(){return m_pAlphaBlendStateManager;}
    PEDepthStencilStateManager *getDepthStencilStateManager(){return m_pDepthStencilStateManager;}
	PE::MemoryArena getDefaultMemoryArena(){return m_defaultArena;}
	Components::Log *getLog(){return m_pLog;}
	Components::NetworkManager *getNetworkManager(){return m_pNetworkManager;}
	Components::MeshManager *getMeshManager(){return m_pMeshManager;}
	Components::GameObjectManager *getGameObjectManager(){return m_pGameObjectManager;}
	Components::DefaultGameControls *getDefaultGameControls(){return m_pDefaultGameControls;}
	unsigned short getLuaCommandServerPort(){return m_luaCommandServerPort;}
	bool getIsServer(){return m_isServer;}
	template <typename T>
	T *get(){return (T*)(m_pGameSpecificContext);}
	
	Components::Component *m_pGame;
	Components::LuaEnvironment *m_pLuaEnv;
	MainFunctionArgs *m_pMPArgs;
	Application *m_pApplication;
	IRenderer *m_pGPUScreen;
	PERasterizerStateManager *m_pRaterizerStateManager;
    PEAlphaBlendStateManager *m_pAlphaBlendStateManager;
    PEDepthStencilStateManager *m_pDepthStencilStateManager;
	Components::Log *m_pLog;
	Components::NetworkManager *m_pNetworkManager;
	Components::GameObjectManager *m_pGameObjectManager;
	Components::MeshManager *m_pMeshManager;
	PE::MemoryArena m_defaultArena;
	unsigned short m_luaCommandServerPort;
	bool m_isServer;
	void * m_pGameSpecificContext; // used to extend this structure
	Components::DefaultGameControls *m_pDefaultGameControls;
	int m_gameThreadThreadOwnershipMask;
};


}; // namespace PE

#endif