#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "StreamManager.h"

// Outer-Engine includes

// Inter-Engine includes

#include "../Lua/LuaEnvironment.h"

// additional lua includes needed
extern "C"
{
#include "../../luasocket_dist/src/socket.h"
#include "../../luasocket_dist/src/inet.h"
};

#include "../../../GlobalConfig/GlobalConfig.h"
#include "PrimeEngine/Events/StandardEvents.h"

// Sibling/Children includes
#include "EventManager.h"
#include "ConnectionManager.h"

#if APIABSTRACTION_PS3
#define NET_LITTLE_ENDIAN 1
#else
#define NET_LITTLE_ENDIAN 0
#endif

using namespace PE::Events;

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(StreamManager, Component);

StreamManager::StreamManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself)
: Component(context, arena, hMyself)
, m_transmissionRecords(1024)
{
	m_nextIdToTransmit = 0;
	m_pNetContext = &netContext;
}

StreamManager::~StreamManager()
{

}

void StreamManager::initialize()
{

}


void StreamManager::sendNextPackets()
{
    while (true)
    {
        int size = PE_PACKET_HEADER; // space for size
        int sizeLeft = PE_PACKET_TOTAL_SIZE - size;

        // allocate data for next packet
	
        int numEvents = m_pNetContext->getEventManager()->haveEventsToSend();

        //todo: other managers
        int numGhosts = 0;

        if (numEvents || numGhosts) //todo: other managers
        {
            m_transmissionRecords.push_back(TransmissionRecord());
            TransmissionRecord &record = m_transmissionRecords.back();
        

            PE::Packet *pPacket = (PE::Packet *)(pemalloc(m_arena, PE_PACKET_TOTAL_SIZE));

            bool usefulEventDataSent = false;
            bool wantToSendMoreEvents = false;
            //event manager
            {
                sizeLeft = PE_PACKET_TOTAL_SIZE - size;
                size += m_pNetContext->getEventManager()->fillInNextPacket(&pPacket->m_data[size], &record, sizeLeft, usefulEventDataSent, wantToSendMoreEvents);
            }

            //other managers fillin here
		
            bool usefulGhostDataSent = false;
            bool wantToSendMoreGhosts = false;
            
            // ghost manager
            {
                sizeLeft = PE_PACKET_TOTAL_SIZE - size;
                size += 0;//m_pNetContext->getEventManager()->fillInNextPacket(&pPacket->m_data[size], &record, sizeLeft, usefulGhostDataSent, wantToSendMoreGhosts);
            }

            assert(size > PE_PACKET_HEADER);// we should have filled in something!
            if (size > PE_PACKET_HEADER)
            {
                StreamManager::WriteInt32(size, &pPacket->m_data[0] /*= &pPacket->m_packetDataSizeInInet*/); // header was allocated in the beginning
                record.m_id = ++m_nextIdToTransmit;
                m_pNetContext->getConnectionManager()->sendPacket(pPacket, &record); // note, we currently have automatic fake acknowldgements so transmission record will be popped in this method
            }
            else
            {
                PEASSERT(false, "Though we had something to send yet no useful data was filled in into packet! events expected: %d event data sent: %d ghosts expected: %d ghost data sent :%d", numEvents, (int)(usefulEventDataSent), numGhosts, (int)(usefulGhostDataSent));
            
                m_transmissionRecords.pop_back(); // cleanup failed transmission record
            }
		
            pefree(m_arena, pPacket);
            
            if (!wantToSendMoreEvents && !wantToSendMoreGhosts)
                return;
        }
        else
        {
            return;
        }
    }
}

void StreamManager::processNotification(bool delivered)
{
	TransmissionRecord &record = m_transmissionRecords.front();


	m_pNetContext->getEventManager()->processNotification(&record, delivered);
	
    // todo: other managers here..

	m_transmissionRecords.pop_front();
}


void StreamManager::do_UPDATE(Events::Event *pEvt)
{
	sendNextPackets();
}

void StreamManager::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, StreamManager::do_UPDATE);
}

#if 0 // template
//////////////////////////////////////////////////////////////////////////
// ConnectionManager Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void ConnectionManager::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	/*
	static const struct luaL_Reg l_functions[] = {
		{"l_clientConnectToTCPServer", l_clientConnectToTCPServer},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_functions);
	*/

	lua_register(luaVM, "l_clientConnectToTCPServer", l_clientConnectToTCPServer);


	// run a script to add additional functionality to Lua side of Skin
	// that is accessible from Lua
// #if APIABSTRACTION_IOS
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code/PrimeEngine/Scene/Skin.lua");
// #else
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code\\PrimeEngine\\Scene\\Skin.lua");
// #endif

}

int ConnectionManager::l_clientConnectToTCPServer(lua_State *luaVM)
{
	lua_Number lPort = lua_tonumber(luaVM, -1);
	int port = (int)(lPort);

	const char *strAddr = lua_tostring(luaVM, -2);

	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -3));

	lua_pop(luaVM, 3);

	pContext->getConnectionManager()->clientConnectToTCPServer(strAddr, port);

	return 0; // no return values
}
#endif

// Sending functionality

void StreamManager::receivePacket(Packet *pPacket)
{
	int read = 0;

	PrimitiveTypes::Int32 packetSize;
	read += StreamManager::ReadInt32(&pPacket->m_data[read], packetSize);

	// events are packed first
	read += m_pNetContext->getEventManager()->receiveNextPacket(&pPacket->m_data[read]);

	assert(packetSize == read);
}



//////////////////////////////////////////////////////////////////////////
// utils

void swapEndian32(void *val)
{
	unsigned int *ival = (unsigned int *)val;
	*ival = ((*ival >> 24) & 0x000000ff) |
		((*ival >>  8) & 0x0000ff00) |
		((*ival <<  8) & 0x00ff0000) |
		((*ival << 24) & 0xff000000);
}


int StreamManager::WriteInt32(PrimitiveTypes::Int32 v, char *pDataStream)
{
	#if !NET_LITTLE_ENDIAN 
		swapEndian32(&v);
	#endif
	
	memcpy(pDataStream, &v, sizeof(PrimitiveTypes::Int32));
	return sizeof(PrimitiveTypes::Int32);
}

int StreamManager::ReadInt32(char *pDataStream, PrimitiveTypes::Int32 &out_v)
{
	PrimitiveTypes::Int32 v;
	memcpy(&v, pDataStream, sizeof(PrimitiveTypes::Int32));

	#if !NET_LITTLE_ENDIAN 
		swapEndian32(&v);
	#endif

	out_v = v;

	return sizeof(PrimitiveTypes::Int32);
}

int StreamManager::WriteFloat32(PrimitiveTypes::Float32 v, char *pDataStream)
{
	#if !NET_LITTLE_ENDIAN 
		swapEndian32(&v);
	#endif

	memcpy(pDataStream, &v, sizeof(PrimitiveTypes::Float32));
	return sizeof(PrimitiveTypes::Float32);
}

int StreamManager::ReadFloat32(char *pDataStream, PrimitiveTypes::Float32 &out_v)
{
	PrimitiveTypes::Float32 v;
	memcpy(&v, pDataStream, sizeof(PrimitiveTypes::Float32));

	#if !NET_LITTLE_ENDIAN 
		swapEndian32(&v);
	#endif

	out_v = v;

	return sizeof(PrimitiveTypes::Float32);
}

int StreamManager::WriteVector4(Vector4 v, char *pDataStream)
{
	int size = 0;
	size += WriteFloat32(v.m_x, &pDataStream[size]);
	size += WriteFloat32(v.m_y, &pDataStream[size]);
	size += WriteFloat32(v.m_z, &pDataStream[size]);
	size += WriteFloat32(v.m_w, &pDataStream[size]);
	return size;
}

int StreamManager::ReadVector4(char *pDataStream, Vector4 &out_v)
{
	int read = 0;
	read += ReadFloat32(&pDataStream[read], out_v.m_x);
	read += ReadFloat32(&pDataStream[read], out_v.m_y);
	read += ReadFloat32(&pDataStream[read], out_v.m_z);
	read += ReadFloat32(&pDataStream[read], out_v.m_w);
	return read;
}


int StreamManager::WriteMatrix4x4(const Matrix4x4 &v, char *pDataStream)
{
	int size = 0;
	for (int i = 0; i < 16; ++i)
		size += WriteFloat32(v.m16[i], &pDataStream[size]);
	return size;
}

int StreamManager::ReadMatrix4x4(char *pDataStream, Matrix4x4 &out_v)
{
	int read = 0;
	for (int i = 0; i < 16; ++i)
		read += ReadFloat32(&pDataStream[read], out_v.m16[i]);
	return read;
}

int StreamManager::WriteNetworkId(Networkable::NetworkId v, char *pDataStream)
{
	return WriteInt32(v, pDataStream);
}

int StreamManager::ReadNetworkId(char *pDataStream, Networkable::NetworkId &out_v)
{
	PrimitiveTypes::Int32 v;
	int res = ReadInt32(pDataStream, v);
	out_v = v;
	return res;
}

	
}; // namespace Components
}; // namespace PE
