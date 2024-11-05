#include "Log.h"

#include "../Lua/LuaEnvironment.h"

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(Log, Component);

Log::Log(PE::GameContext &context, PE::MemoryArena arena, Handle m_hMyself, const char* tagName):Component(context, arena, m_hMyself), m_tagName(tagName), m_activateOnNextFrame(false), m_isActivated(false)
{
}

void Log::handleEvent(Events::Event *pEvt)
{
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	if(m_isActivated)
	{
		char msg[128];
		char handleNumberString [128];

		StringOps::intToStr((pEvt->m_lastDistributor.m_memoryPoolIndex * MAX_NUM_BLOCKS_PER_POOL) + pEvt->m_lastDistributor.m_memoryBlockIndex, handleNumberString, 128); //Convert caller's handle number to string

		//StringOps::concat(EventToStrMap::Instance()->findString(pEvt->m_type), m_tagName,  msg, 128); //Combine event name with tag
		StringOps::concat(msg,pEvt->m_lastDistributor.getDbgName(), msg, 128); //Combine above with handle type
		StringOps::concat(msg, " ", msg, 128); //Add a space
		StringOps::concat(msg, handleNumberString, msg, 128); //Combine above with number representation of handle
		
		PEINFOSTR((LPCSTR)msg);
	}
#endif
}

void Log::printDebugInt(PrimitiveTypes::Int32 n)
{
	char msg[128];

	StringOps::intToStr(n, msg, 128); 
	PEINFOSTR(msg);
}

// Methods --------------------------------------------------------------

}; // namespace Components
}; // namespace PE

