#ifndef __PYENGINE_2_0_SOUND_MANAGER__
#define __PYENGINE_2_0_SOUND_MANAGER__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "../Events/Component.h"
#include "../Events/Event.h"
#include "../Events/StandardEvents.h"


// Sibling/Children includes
namespace PE {
namespace Components {

struct SoundManager : public Component
{
	PE_DECLARE_CLASS(SoundManager);

	SoundManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, const char* wbFilename, bool isActive = true): Component(context, arena, hMyself)
	{
		#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
		#endif
	}

	virtual ~SoundManager() {}
	
	// Component ------------------------------------------------------------

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();
		
		PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, SoundManager::do_UPDATE);
	}

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt)
	{
#		if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
#		endif
	}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena, const char* wbFilename, bool isActive = true)
	{
		Handle h("SOUND_MANAGER", sizeof(SoundManager));
		SoundManager *pSoundManager = new(h) SoundManager(context, arena, h, wbFilename, isActive);
		pSoundManager->addDefaultComponents();

		SetInstance(h);
		s_isActive = isActive;
	}
	
	static void SetInstance(Handle h){s_hInstance = h;}

	static SoundManager *Instance() {return s_hInstance.getObject<SoundManager>();}
	static Handle InstanceHandle() {return s_hInstance;}
	static Handle s_hInstance;
	static bool s_isActive;

	#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
	Handle m_hXACT;
	
	#endif
};
}; // namespace Components
}; // namespace PE
#endif
