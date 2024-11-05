#ifndef __PYENGINE_2_0_EVENTHANDLER_H__
#define __PYENGINE_2_0_EVENTHANDLER_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes

#include "PrimeEngine/Utils/PEUUID.h"

#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Utils/PEString.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/Utils/PEClassDecl.h"

#include "PrimeEngine/Lua/typedefs.h"

// Sibling/Children includes
#include "Event.h"


namespace PE {

namespace Components{

struct Component : public PEClass
{
	PE_DECLARE_CLASS(Component);

	// meta methods: wrap around MetaInfo methods
	int getClassId() { return getClassMetaInfo()->m_classId; }
	const char *getClassName() { return getClassMetaInfo()->getClassName(); }

	template <typename T>
	bool isInstanceOf() {
		return getClassMetaInfo()->isSubClassOf(T::s_metaInfo.m_classId);
	}

	bool isInstanceOf(int classId) {
		return getClassMetaInfo()->isSubClassOf(classId);
	}

	void printClassHierarchy()
	{
		return getClassMetaInfo()->printClassHierarchy();
	}

	virtual ~Component(){}

	Handle getHandle(){return m_hMyself;}
	GameContext *getGameContext(){return m_pContext;}

	bool isEnabled()const {return m_enabled;}
	void setEnabled(bool val){m_enabled = val;}
	//////////////////////////////////////////////////////////////////////////

	typedef void (Component::*HandlerMethod) (Events::Event *);

	typedef void (*StaticHandlerMethod) (Events::Event *, Handle &);

	Component(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual void addDefaultComponents();

	virtual void handleEvent(Events::Event *pEvt);

	void passEventToLuaCompTable(Events::Event *pEvt);
	
	void removeComponents(int classId);
	void removeComponent(Handle hChild);
	virtual void removeComponent(int index);

	virtual void addComponent(Handle hComponent, int *pAllowedEvents = NULL);

	PrimitiveTypes::Int32 getFirstComponentIndex(int classId, PrimitiveTypes::UInt32 startIndex = 0)
	{
		Handle *cIter = m_components.getFirstPtr();
		cIter += startIndex;
		for (PrimitiveTypes::UInt32 i = startIndex; i < m_components.m_size; i++)
		{
			if (cIter->getObject<Component>()->isInstanceOf(classId))
			{
				return i;
			}
			++cIter;
		}
		return -1;
	}
	template <typename T> PrimitiveTypes::Int32 getFirstComponentIndex(PrimitiveTypes::UInt32 startIndex = 0) { return getFirstComponentIndex(T::GetClassId(), startIndex); }


	PrimitiveTypes::Bool getFirstComponentIH(int classId, PrimitiveTypes::UInt32 startIndex, PrimitiveTypes::Int32 &out_index, Handle &out_handle)
	{
		out_index = getFirstComponentIndex(classId, startIndex);
		if (out_index != -1)
		{
			out_handle = m_components[out_index];
			return true;
		}
		return false;
	}
	template <typename T> PrimitiveTypes::Bool getFirstComponentIH(PrimitiveTypes::UInt32 startIndex, PrimitiveTypes::Int32 &out_index, Handle &out_handle) { return getFirstComponentIH(T::GetClassId(), startIndex, out_index, out_handle); }


	template <typename T>
	PrimitiveTypes::Bool getFirstComponentIP(PrimitiveTypes::UInt32 startIndex, PrimitiveTypes::Int32 &out_index, T *&out_ptr)
	{
		out_index = getFirstComponentIndex(T::GetClassId(), startIndex);
		if (out_index != -1)
		{
			out_ptr = m_components[out_index].getObject<T>();
			return true;
		}
		return false;
	}


	Handle getFirstComponentHandle(int classId, PrimitiveTypes::UInt32 startIndex = 0)
	{
		PrimitiveTypes::Int32 i = getFirstComponentIndex(classId, startIndex);
		return i != -1 ? m_components[i] : Handle();
	}
	template <typename T> Handle getFirstComponentHandle(PrimitiveTypes::UInt32 startIndex = 0) {return getFirstComponentHandle(T::GetClassId(), startIndex);}

	
	template <typename T>
	T *getFirstComponent() { 
		Handle h =  getFirstComponentHandle(T::GetClassId());
		return h.isValid() ? h.getObject<T>() : 0; 
	}

	Handle getComponentByIndex(PrimitiveTypes::UInt32 index)
	{
		return index < m_components.m_size ? m_components[index] : Handle();
	}
	
	template <typename T>
	T* getComponentByIndex(PrimitiveTypes::UInt32 index) { return getComponentByIndex(index).getObject<T>(); }

	PrimitiveTypes::UInt32 countComponents(int classId);
	template <typename T> PrimitiveTypes::UInt32 countComponents() {return countComponents(T::GetClassId());}

	static Handle getFirstComponentHandle(Handle hObj, int classId) { return hObj.getObject<Component>()->getFirstComponentHandle(classId); }
	template <typename T> 
	static Handle getFirstComponentHandle(Handle hObj, int classId) { return hObj.getObject<Component>()->getFirstComponentHandle(classId); }
	
	template <typename T>
	static Handle getFirstComponentHandle(Handle hObj) { return getFirstComponentHandle(hObj, T::GetClassId()); }
	
	template <typename T>
	static T *getFirstComponent(Handle hObj) { return hObj.getObject<Component>()->getFirstComponent<T>(); }


	
	Handle getFirstComponentRev(int classId)
	{
		for (PrimitiveTypes::Int32 i = m_components.m_size - 1; i >= 0; --i)
		{
			Handle cur = m_components[i];
			if (cur.getObject<Component>()->isInstanceOf(classId))
			{
				return cur;
			}
		}
		return Handle();
	}

	PrimitiveTypes::UInt32 getComponentCount() {return m_components.m_size;}

	Handle getFirstParentByType(int classId);
	template <typename T> Handle getFirstParentByType() {return getFirstParentByType(T::GetClassId());}
	
	template <typename T> T* getFirstParentByTypePtr() {
		Handle h = getFirstParentByType<T>();
		return h.getObject<T>();
	}

	void addParent(Handle parent, int *pAllowedEventsToPropagateToParent);

	void distributeEvtToAllComponents(Events::Event *pEvt)
	{
		Handle prevDistributor = pEvt->m_lastDistributor;
		PrimitiveTypes::UInt32 returnCode = pEvt->m_returnCode;

		Handle *hiter = m_components.getFirstPtr();
		for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++)
		{
			Component *pObj = hiter->getObject<Component>();
			pEvt->m_lastDistributor = m_hMyself;
			pObj->handleEvent(pEvt);
			if (pEvt->m_returnCode == Events::STOP_DISTRIBUTION)
				break;
			++hiter;
		}
	

		/*
		pEvt->m_lastDistributor = m_hMyself;
		passEventToLuaCompTable(pEvt);
		*/
		pEvt->m_lastDistributor = prevDistributor;
		pEvt->m_returnCode = returnCode;
	}

	void distributeEvtToQueue(Events::Event *pEvt)
	{
		Handle prevDistributor = pEvt->m_lastDistributor;
		PrimitiveTypes::UInt32 returnCode = pEvt->m_returnCode;

		pEvt->m_lastDistributor = m_hMyself;
		passEventToLuaCompTable(pEvt);

		pEvt->m_lastDistributor = prevDistributor;
		pEvt->m_returnCode = returnCode;
	}

	void createLuaCompTableIfDoesntExist(lua_State *L);
	void putCompLuaTableOnStack(lua_State *L);
	bool putHandlingQueueOnStack(int evtClassId);
	// returns true if created a new queue
	bool putOrCreateHandlingQueueOnStack(int evtClassId);
	void createHandlingQueueAsField(int evtClassId);
	void popHandlingQueue();
	void popCompLuaTable();
	void addComponentToHandlerQueue(int evtClassId, Handle hComponent);
	
	
	void _addMethodToHandlerQueue(int evtClassId, HandlerMethod method);

	void _addStaticMethodToHandlerQueue(int evtClassId, StaticHandlerMethod method);

	


	void propagateEventHandlersToParents();
	void propagateEventHandlersToParent(Handle hParent, int *pAllowedEvents);

	
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_ADDED_AS_COMPONENT);
	virtual void do_ADDED_AS_COMPONENT(Events::Event *pEvt);

	//////////////////////////////////////////////////////////////////////////
	// Component Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_GetHandlerQueueTable(lua_State *luaVM);
	//
	// returns a list (Lua table) of handles of all child components of the given component
	static int l_GetComponentHandles(lua_State *luaVM);
	//
	static int l_GetComponentInfo(lua_State *luaVM);
	//
	static int l_SendEventToHandle(lua_State* luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

	static Handle s_debuggedComponent;
	static int s_debuggedEvent;

protected:

	Array<Handle, 1> m_components; // could be anuything. Basically event handlers. could be scene nodes, models, etc.
	Handle m_hMyself; // handle to itself
	PrimitiveTypes::Bool m_breakExecturion;
	Array<Handle, 1> m_parents; //Parents
	Array<int *, 1> m_allowedComponentEventsToParents; // events allowed to propagate to parents

	
	LuaGlue::LuaReference m_luaCompTableRef;
	PrimitiveTypes::Bool m_enabled;
	PE::MemoryArena m_arena;
	PE::GameContext *m_pContext;

};
    
        
    
}; // namespace Components
}; // namespace PE
#endif
