#ifndef _PE_Networkable_
#define _PE_Networkable_

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"



#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"

#include "PrimeEngine/Networking/NetworkContext.h"
#include "PrimeEngine/Utils/PEClassDecl.h"



#define PE_DECLARE_NETWORKABLE_CLASS \
	/* methods to allow to know what class this networkable is networking over */ \
	virtual PE::MetaInfo *net_getClassMetaInfo() {return getClassMetaInfo();} \
	/* override the function to register factory function */ \
	static void SetNetworkingFunctions() {s_metaInfo.m_factoryConstructFunction = NULL;}


namespace PE {
	struct GameContext;
namespace Components {
	struct NetworkManager;
};

struct Networkable
{
	typedef unsigned int NetworkId;

	typedef void (*StaticConstructMethod) (PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, const char *pDataStream);

	Networkable(GameContext &context, void *pMainClassThis, NetworkId networkId = s_NetworkId_Invalid);
	virtual ~Networkable(){}

	void registerWithNetwork(PE::Components::NetworkManager *pNetworkManager);
	
	// used in all classes that can be created over network
	// events use inly this method and no other ones since events are instantaneous objects
	virtual int packCreationData(char *pDataStream){assert(!"This function is not overridden by this class and should not be called! Make sure to implement override of this function in current class"); return 0;}

	// usually used in game classes
	virtual int packStateData(char *pDataStream){assert(!"This function is not overridden by this class and should not be called! Make sure to implement override of this function in current class"); return 0;}

	// methods to allow to know what class this networkable is networking over
	virtual PE::MetaInfo *net_getClassMetaInfo() = 0;

	void *getPointerToMainClass(){return m_pMainClassThis;}

	NetworkId m_networkId;


	// preassigned ids
	static NetworkId s_NetworkId_Invalid;
	static NetworkId s_NetworkId_GameObjectManager;
	static NetworkId s_NetworkId_LuaEnvironment;
	static NetworkId s_NetworkId_NetworkManager;


	//first dynamic id, after this can allocated ids dynamically
	static NetworkId s_NetworkId_FirstDynamic;

	void *m_pMainClassThis; // a pointer to main class in multiple inheritance stack
};


}; // namespace PE

#endif
