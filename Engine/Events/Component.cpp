// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../../../GlobalConfig/GlobalConfig.h"

// Sibling/Children includes
#include "Component.h"
#include "../Logging/Log.h"
#include "../Utils/PEUUID.h"

#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardGameEvents.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"

namespace PE {
	GlobalRegistry GlobalRegistry::s_pInstance;


using namespace Events;

namespace Components {



PE_IMPLEMENT_CLASS0(Component);

Handle Component::s_debuggedComponent;
int Component::s_debuggedEvent = 0;

Component::Component(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :
	m_hMyself(hMyself),
	m_breakExecturion(false),
	m_components(context, arena, 1024),
	m_parents(context, arena, 1024),
	m_allowedComponentEventsToParents(context, arena, 1024),
	m_luaCompTableRef(LUA_NOREF),
	m_enabled(true)
{
	if (!hMyself.isValid()) // in case handle wasnt passed in (placement operator new was used) then we create handle for ourself here
	{
		m_hMyself = Handle(this);
	}

	m_arena = arena; m_pContext = &context;
}

void Component::addDefaultComponents()
{
	PE::MetaInfo *classMetaInfo = getClassMetaInfo();
	if (classMetaInfo->m_classId == -1)
	{
		assert(!"Class Is not Registered. Have you forgotten to put it in GlobalRegistry?");
	}

	createLuaCompTableIfDoesntExist(m_pContext->getLuaEnvironment()->L);
    
	PE_REGISTER_EVENT_HANDLER(PE::Events::Event_ADDED_AS_COMPONENT, Component::do_ADDED_AS_COMPONENT);
	

	addComponent(Handle(m_pContext->getLog())); // create temp handle that just stores pointer
}

// this method should not be overriden if possible!
void Component::handleEvent(Event *pEvt)
{
	if (!m_enabled)
		return;
	// we have just received this event
	// need to push the distributor stack
	// and set myself as last distributor

	Handle cachedPrevDistributor = pEvt->m_prevDistributor;
	pEvt->m_prevDistributor = pEvt->m_lastDistributor;
	pEvt->m_lastDistributor = m_hMyself;

	distributeEvtToQueue(pEvt);

	// this is old behavior that is left here for reference
	// do not uncomment this
	//distributeEvtToAllComponents(pEvt);


	// pop the distributor stack
	pEvt->m_lastDistributor = pEvt->m_prevDistributor;
	pEvt->m_prevDistributor = cachedPrevDistributor;
}

Handle Component::getFirstParentByType(int classId)
{
	Handle *hparentIter = m_parents.getFirstPtr();
	for (PrimitiveTypes::UInt32 i = 0; i < m_parents.m_size; i++)
	{
		if (hparentIter->getObject<Component>()->isInstanceOf(classId))
		{ 
			return *hparentIter;
		}
		++hparentIter;
	}
	return Handle();
}

// Component management

void Component::addComponent(Handle hComponent, int *pAllowedEvents /* = NULL */)
{
	assert(hComponent.isValid());

	PEASSERT(hComponent.getObject<Component>()->isEnabled(), "If teh comonent is disabled it will misss crucial events whe child is added, this is not probably what we want");

	m_components.add(hComponent);
	
	if(!(hComponent == m_hMyself))
	{
		// send event back to the child components
        Events::Event_ADDED_AS_COMPONENT evt;
		
		evt.m_hComponentParent = m_hMyself;
		evt.m_lastDistributor = m_hMyself;
		evt.m_target = hComponent;
		evt.m_pAllowedEventsToPropagateToParent = pAllowedEvents;

		hComponent.getObject<Component>()->handleEvent(&evt);
		// make sure we regiter the component with all events
		hComponent.getObject<Component>()->propagateEventHandlersToParents();
	}
}

void Component::addParent(Handle parent, int *pAllowedEventsToPropagateToParent)
{
	m_parents.add(parent);
	m_allowedComponentEventsToParents.add(pAllowedEventsToPropagateToParent);
}

void Component::removeComponents(int classId)
{
	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++)
	{
		Handle cur = m_components[i];
		if (cur.getObject<Component>()->isInstanceOf(classId))
		{
			m_components.remove(i);
			i--;
		}
	}
}

PrimitiveTypes::UInt32 Component::countComponents(int classId)
{
	PrimitiveTypes::UInt32 res = 0;
	Handle *cIter = m_components.getFirstPtr();
	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++)
	{
		if (cIter->getObject<Component>()->isInstanceOf(classId))
		{
			res++;
		}
		++cIter;
	}
	return res;
}

void Component::removeComponent(Handle hChild)
{
	PrimitiveTypes::UInt32 index = m_components.indexOf(hChild);
	// TODO: send event that it was removed
	if (index != PrimitiveTypes::Constants::c_MaxUInt32)
	{
		removeComponent(index);
	}
}

void Component::removeComponent(int index)
{
	if (index != PrimitiveTypes::Constants::c_MaxUInt32)
	{
		m_components.remove(index);
	}
}

// this function can be called with root lua_State or tread substates
void Component::createLuaCompTableIfDoesntExist(lua_State *L)
{
	if (m_luaCompTableRef == LUA_NOREF)
		m_luaCompTableRef = LuaEnvironment::createTableOnTopOfStackAndStoreReference(L);
}

// this function can be called with root lua_State or tread substates
void Component::putCompLuaTableOnStack(lua_State *L)
{
	if (m_luaCompTableRef != LUA_NOREF)
	{
		LuaEnvironment::putTableOnTopOfStackByReference(m_luaCompTableRef, L);
	}
	else
	{
		createLuaCompTableIfDoesntExist(L);
		assert(m_luaCompTableRef != LUA_NOREF);
		LuaEnvironment::putTableOnTopOfStackByReference(m_luaCompTableRef, L);
	}
}

bool Component::putHandlingQueueOnStack(int evtClassId)
{
	return m_pContext->getLuaEnvironment()->checkTableValueByInt32KeyExists(evtClassId);
}

void Component::popHandlingQueue()
{
	m_pContext->getLuaEnvironment()->pop();
}

void Component::popCompLuaTable()
{
	m_pContext->getLuaEnvironment()->pop();
}

bool Component::putOrCreateHandlingQueueOnStack(int evtClassId)
{
	bool res = false;
	if (!putHandlingQueueOnStack(evtClassId))
	{
		// the table does not exist, so nil is on top. need to pop and add new table field
		res= true;
		popHandlingQueue();
		createHandlingQueueAsField(evtClassId);
		bool success = putHandlingQueueOnStack(evtClassId);
		assert(success); // now the table has to exist
	}
	return res;
}

void Component::createHandlingQueueAsField(int evtClassId)
{
	m_pContext->getLuaEnvironment()->pushNewTableAsFieldKeyedByInt32(evtClassId);
}

void Component::addComponentToHandlerQueue(int evtClassId, Handle hComponent)
{
	putCompLuaTableOnStack(m_pContext->getLuaEnvironment()->L);
	putOrCreateHandlingQueueOnStack(evtClassId);

	bool added = LuaEnvironment::pushHandleAsNextArrayElementIfNotInArray(m_pContext->getLuaEnvironment()->L, hComponent);

	popHandlingQueue();

	popCompLuaTable();
	if (added)
	{
		// need to notify parents that new event needs to be passed
		propagateEventHandlersToParents();
	}
}

void Component::_addMethodToHandlerQueue(int evtClassId, Component::HandlerMethod method)
{
	putCompLuaTableOnStack(m_pContext->getLuaEnvironment()->L);
	putOrCreateHandlingQueueOnStack(evtClassId);

	bool added = LuaEnvironment::pushMethodAsNextArrayElementIfNotInArray(m_pContext->getLuaEnvironment()->L, method);

	popHandlingQueue();

	popCompLuaTable();
	if (added)
	{
		// need to notify parents that new event needs to be passed
		propagateEventHandlersToParents();
	}
}
void Component::_addStaticMethodToHandlerQueue(int evtClassId, Component::StaticHandlerMethod method)
{
	if (evtClassId == -1)
		assert(!"Event registering handler for has not been registered. Do you need to add it to global registry?");

	putCompLuaTableOnStack(m_pContext->getLuaEnvironment()->L);
	putOrCreateHandlingQueueOnStack(evtClassId);

	bool added = LuaEnvironment::pushStaticMethodAsNextArrayElementIfNotInArray(m_pContext->getLuaEnvironment()->L, method);
	if (!added)
	{
		#if PE_DONT_ALLOW_MULTIPLE_METHODS_IN_EVENT_PROCESSING_QUEUE
			assert(!"Method is already registered or trying to add multiple methods in event processing queue. This should not happen");
		#else
			assert(!"Method is already registered. This should not happen");
		#endif
	}

	popHandlingQueue();

	popCompLuaTable();
	if (added)
	{
		// need to notify parents that new event needs to be passed
		propagateEventHandlersToParents();
	}
}
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS
#define BREAK_HERE() __asm __emit 0xF1
#else
#define BREAK_HERE()
#endif
#define BREAKABLE(declaration) declaration \
{BREAK_HERE(); //

void Component::passEventToLuaCompTable(Event *pEvt)
{
	putCompLuaTableOnStack(m_pContext->getLuaEnvironment()->L);
	if (putHandlingQueueOnStack(pEvt->getClassId()))
	{
		// TODO: add ability to debug the event handling here
		// when PyClient tells the engine to debug here
		// this component has handler(s) for this event so we know it will be processed
		//if (s_debuggedComponent == m_hMyself)
		//{
		//	s_debuggedComponent = Handle();
		//	BREAK_HERE();
		//}
		m_pContext->getLuaEnvironment()->passEventToQueue(pEvt);
	}
	popHandlingQueue(); // even if the table was not pushed. nill vas pushed so we need to pop
	popCompLuaTable();
}

void Component::propagateEventHandlersToParents()
{
	Handle *p = m_parents.getFirstPtr();
	int **ppAllowedEvents = m_allowedComponentEventsToParents.getFirstPtr();
	for (PrimitiveTypes::UInt32 i = 0; i < m_parents.m_size; ++i)
	{
		propagateEventHandlersToParent(*p, *ppAllowedEvents);
		++p;
		++ppAllowedEvents;
	}
}

void Component::propagateEventHandlersToParent(Handle hParent, int *pAllowedEvents)
{
	putCompLuaTableOnStack(m_pContext->getLuaEnvironment()->L);

	Array<PrimitiveTypes::Int32> eventsCanHandle(*m_pContext, m_arena, 0);

	m_pContext->getLuaEnvironment()->iterateOverTableAndGetEventTypes(eventsCanHandle);

	popCompLuaTable();

	Component *pParent = hParent.getObject<Component>();
	
	PrimitiveTypes::Int32 *p = eventsCanHandle.getFirstPtr();
	for (PrimitiveTypes::UInt32 i = 0; i < eventsCanHandle.m_size; ++i)
	{
		bool allowed = pAllowedEvents == NULL;
		if (pAllowedEvents)
		{
			while (*(pAllowedEvents++) != 0)
			{
				// there is a list of allowed event handlers this component can handle (taht is we are restricting some handlers, this is useful for circular component dependencies)
				// for example if two components want to be registered for each others events
				// some events (like UPDATE) have to be restricted so that we dont get into infinite loops
			
				if (*pAllowedEvents == (int)(i))
				{
					allowed = true;
					break;
				}
			}
		}
		if (allowed)
			pParent->addComponentToHandlerQueue(*p, m_hMyself);
		p++;
	}

	eventsCanHandle.reset(0);
}

void Component::do_ADDED_AS_COMPONENT(Events::Event *pEvt)
{

	Events::Event_ADDED_AS_COMPONENT *pRealEvent = (Events::Event_ADDED_AS_COMPONENT *)(pEvt);

	if (pEvt->m_target == m_hMyself)
	{
		PEASSERT(pEvt->m_target.getObject<Component>() == this, "Invalid handle");
		addParent(pRealEvent->m_hComponentParent, pRealEvent->m_pAllowedEventsToPropagateToParent);
	}
	else
	{
		// this probably means that this object in the queue for the event and gets it because its parent got it
		// for example if this is a mesh in a scene node. and then a scene node is added somewhere,
		// the event will be sent to the scene node, but it will also be propagated to all registered objects
		// this could be useful in case a component wants to track if its parent is added
		// but default behavior is to ignore.
	}

}


//////////////////////////////////////////////////////////////////////////
// Component Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void Component::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Component[] = {
		{"GetHandlerQueueTable", l_GetHandlerQueueTable},
		{"GetComponentHandles", l_GetComponentHandles},
		{"GetComponentInfo", l_GetComponentInfo},
		{"l_SendEventToHandle", l_SendEventToHandle}, // will be wrapped by Lua function SendEventToHandle
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_MOVE
	luaL_register(luaVM, 0, l_Component);
#if APIABSTRACTION_IOS
	pLuaEnv->runScriptWorkspacePath("Code/PrimeEngine/Events/Component.lua");
#else
    pLuaEnv->runScriptWorkspacePath("Code\\PrimeEngine\\Events\\Component.lua");
#endif
}
//
int Component::l_GetHandlerQueueTable(lua_State *luaVM)
{
	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Components::Component *pHandler = h.getObject<Components::Component>();

	if (luaVM != pHandler->m_pContext->getLuaEnvironment()->L)
	{
		// todo: would be nice to check that the luaVM is sub state of this component's context
	}

	pHandler->putCompLuaTableOnStack(luaVM);
	int type = lua_type(luaVM, -1);
	assert(type == LUA_TTABLE);
	return 1;
}
//
int Component::l_GetComponentHandles(lua_State *luaVM)
{
	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Components::Component *pComponent = h.getObject<Components::Component>();
	// return a table of handles(that are tables themselves)
	lua_newtable(luaVM);

	for (PrimitiveTypes::UInt32 ic = 0; ic < pComponent->getComponentCount(); ic++)
	{
		lua_pushnumber(luaVM, ic+1); // index starting at 1
		LuaGlue::pushTableBuiltFromHandle(luaVM, pComponent->getComponentByIndex(ic)); // value
		lua_rawset(luaVM, -3); // set 
	}

	return 1;
}
//
int Component::l_GetComponentInfo(lua_State *luaVM)
{
	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Components::Component *pComp = h.getObject<Components::Component>();

	lua_newtable(luaVM);

	lua_pushstring(luaVM, "className");
	lua_pushstring(luaVM, pComp->getClassName());
	lua_rawset(luaVM, -3); // set 

	return 1;
}
//
int Component::l_SendEventToHandle(lua_State* luaVM)
{
	//Assumes handle to Component to send event to has been pushed, followed by the handle of the event to send
	int x = lua_gettop(luaVM);
	bool b = lua_istable(luaVM, -1);
	int a = lua_type(luaVM, -1);

	Handle hcreateEvent;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, hcreateEvent);

	x = lua_gettop(luaVM);
	b = lua_istable(luaVM, -1);
	a = lua_type(luaVM, -1);

	Handle hComponent;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, hComponent); //For some reason if I call this function table will be gone once I enter its scope. That's why I am just doing the popping here directly

	if (hComponent.isValid())
	{
		hComponent.getObject<Component>()->handleEvent(hcreateEvent.getObject<Events::Event>());
	}
	hcreateEvent.getObject<Events::Event>()->releaseEventData();

	return 0;
}

}; // namespace Components
}; // namespace PE
