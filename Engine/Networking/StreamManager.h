#ifndef __PrimeEngineStreamManager_H__
#define __PrimeEngineStreamManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <deque>

// Inter-Engine includes

#include "../Events/Component.h"

#include "PrimeEngine/Math/Matrix4x4.h"

extern "C"
{
#include "../../luasocket_dist/src/socket.h"
};

#include "PrimeEngine/Networking/NetworkContext.h"
#include "PrimeEngine/Utils/Networkable.h"

// Sibling/Children includes
#include "Packet.h"

namespace PE {
namespace Components {

struct StreamManager : public Component
{
	PE_DECLARE_CLASS(StreamManager);

	// Constructor -------------------------------------------------------------
	StreamManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself);

	virtual ~StreamManager();

	// Methods -----------------------------------------------------------------
	virtual void initialize();

	void sendNextPackets();

	void receivePacket(Packet *pPacket);

	void processNotification(bool delivered);

	static int WriteInt32(PrimitiveTypes::Int32 v, char *pDataStream);
	static int ReadInt32(char *pDataStream, PrimitiveTypes::Int32 &out_v);

	static int WriteFloat32(PrimitiveTypes::Float32 v, char *pDataStream);
	static int ReadFloat32(char *pDataStream, PrimitiveTypes::Float32 &out_v);

	static int WriteVector4(Vector4 v, char *pDataStream);
	static int ReadVector4(char *pDataStream, Vector4 &out_v);

	static int WriteMatrix4x4(const Matrix4x4 &v, char *pDataStream);
	static int ReadMatrix4x4(char *pDataStream, Matrix4x4 &out_v);

	static int WriteNetworkId(Networkable::NetworkId v, char *pDataStream);
	static int ReadNetworkId(char *pDataStream, Networkable::NetworkId &out_v);

	
	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	// Loading -----------------------------------------------------------------

#if 0 // template
	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_clientConnectToTCPServer(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////
#endif
	//////////////////////////////////////////////////////////////////////////
	// Member variables 
	//////////////////////////////////////////////////////////////////////////
	std::deque<TransmissionRecord> m_transmissionRecords;

	//receiving context
	int m_firstIdNotYetReceived;
	
	// sending context
	int m_nextIdToTransmit;
	int m_nextIdToBeAcknowledged;

	PE::NetworkContext *m_pNetContext;
};
}; // namespace Components
}; // namespace PE
#endif
