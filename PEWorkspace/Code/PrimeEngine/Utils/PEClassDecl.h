#ifndef _PE_CLASS_DECL_
#define _PE_CLASS_DECL_

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"



#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"

#include "PrimeEngine/MemoryManagement/MemoryPool.h"

#define PE_MAX_SUPERCLASSES 3
#define MAX_CLASSES 1024

#define PE_USE_VIRTUAL_EVENT_HANDLERS 0
#define PE_USE_STATIC_EVENT_HANDLERS (!PE_USE_VIRTUAL_EVENT_HANDLERS)


struct lua_State;

namespace PE {
	struct GameContext;
	namespace Components{
		struct LuaEnvironment;

	}
	
	struct MetaInfo;

	struct GlobalRegistry
	{
		void *operator new(size_t size, Handle &h)
		{
			// create this object in allocated memory
			return h.getObject();
		}

		void operator delete(void *ptr, Handle &h)
		{
			// don't do anything
		}
		
		GlobalRegistry()
		{
			m_numMetaInfos = 0;
			classIdCounter = 0;
			m_initialized = false;
		}

		void setInitialized(bool v){m_initialized = v;}
		bool getIsInitialized(){return m_initialized;}

		static GlobalRegistry *Instance(){return &s_pInstance;}

		int getNextClassId(MetaInfo *pMetaInfo = NULL)
		{
			int res = classIdCounter;
			m_metaInfos[classIdCounter] = pMetaInfo;
			classIdCounter++;
			return res;
		}

		PE::MetaInfo *getMetaInfo(int classId) { return m_metaInfos[classId];}

		static GlobalRegistry s_pInstance;
		
		int classIdCounter;

		PE::MetaInfo *m_metaInfos[MAX_CLASSES];
		int m_numMetaInfos;

		bool m_initialized;

	};

	struct MetaInfo
	{
		typedef void*(*FactoryConstructFunction)(PE::GameContext&, PE::MemoryArena);

		MetaInfo():m_classId(-1), m_factoryConstructFunction(NULL)
		{}

		virtual ~MetaInfo(){}
		bool isSubClassOf(int classId) {
			if (m_classId == classId)
				return true;

			int i = 0;

			while (m_superClasses[i] != -1) {
				PE::MetaInfo *pSuperClassMetaInfo = GlobalRegistry::Instance()->getMetaInfo(m_superClasses[i]);
				bool res = pSuperClassMetaInfo->isSubClassOf(classId);
				if (res)
					return true;
				i++;
			}
			return false;
		}
		void printClassHierarchy_()
		{
			int i = 0;
			PEINFO("%s : {", getClassName());
			while (m_superClasses[i] != -1) {
				PE::MetaInfo *pSuperClassMetaInfo = GlobalRegistry::Instance()->getMetaInfo(m_superClasses[i]);
				pSuperClassMetaInfo->printClassHierarchy_();
				i++;
			}
			PEINFO("}");
		}

		void printClassHierarchy()
		{
			printClassHierarchy_();
			PEINFO("\n");
		}

		FactoryConstructFunction getFactoryConstructFunction() {return m_factoryConstructFunction;}

		int m_classId;
		int m_superClasses[PE_MAX_SUPERCLASSES+1];

		virtual const char *getClassName() = 0;

		FactoryConstructFunction m_factoryConstructFunction;
	};

#define PE_DECLARE_CLASS(class_) \
	typedef class_ MyClass; \
	static void setSuperClasses(); \
	static void registerClass(PE::GlobalRegistry *pRegistry); \
	static void InitializeAndRegister(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry, bool setLuaMetaDataOnly); \
	static void SetLuaMetaData(PE::Components::LuaEnvironment *pLuaEnv); \
	static MyClass *ConstructInstance(PE::GameContext &context, PE::MemoryArena arena); \
	struct class_ ## MetaInfo : public PE::MetaInfo { \
	virtual const char *getClassName() { return #class_; }; \
	}; \
	static class_ ## MetaInfo s_metaInfo; \
	static int s_classSize; \
	virtual PE::MetaInfo *getClassMetaInfo() {return &s_metaInfo;} \
	static const char *GetClassName() {return s_metaInfo.getClassName();} \
	static int GetClassId() {return s_metaInfo.m_classId;} \
	/* object virtual methods */ \
	virtual int getClassSize() {return s_classSize; }

#define PE_DECLARE_CLASS_FACTORY(class_) \
	static MyClass *ConstructInstance(PE::GameContext &context, PE::MemoryArena arena);


#define PE_DECLARE_SINGLETON_CLASS(class_) \
	PE_DECLARE_CLASS(class_) \
	static PE::Handle s_hMyself; \
	static PE::Handle GetInstanceHandle() {return s_hMyself;} \
	static void SetInstanceHandle(const PE::Handle &h) {s_hMyself = h;} \
	static class_ *GetInstance() { return s_hMyself.isValid() ? s_hMyself.getObject<class_>() : NULL;}

#define PE_IMPLEMENT_CLASS_COMMON(class_) \
	class_::class_ ## MetaInfo class_::s_metaInfo; \
	int class_::s_classSize; \
	void class_::registerClass(PE::GlobalRegistry *pRegistry){ \
        class_::s_metaInfo.m_classId = pRegistry->getNextClassId(&class_::s_metaInfo); \
		class_::s_classSize = sizeof(class_); \
	} \
	void class_::SetLuaMetaData(PE::Components::LuaEnvironment *pLuaEnv) {  \
	/* assumes the namespace table is on lua stack top */ \
		pLuaEnv->pushNewTableAsFieldKeyedByString(GetClassName(), true); \
		pLuaEnv->pushInt32AsFieldAndSet("ClassId", GetClassId()); \
		class_::SetLuaFunctions(pLuaEnv, pLuaEnv->L); \
		pLuaEnv->pop(); \
	} \
	void class_::InitializeAndRegister(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry, bool setLuaMetaDataOnly) { \
		if (!setLuaMetaDataOnly){ \
			setSuperClasses(); /* stores the class hierarchy for this class */ \
			registerClass(pRegistry); \
			class_::s_metaInfo.m_factoryConstructFunction = class_::FactoryConstruct; \
		} \
		SetLuaMetaData(pLuaEnv); \
	}

#define PE_IMPLEMENT_CLASS_FACTORY(class_) \
	class_::MyClass *class_::ConstructInstance(PE::GameContext &context, PE::MemoryArena arena){ \
		MyClass *pNewInstance = new (arena) MyClass(context, arena); \
		return pNewInstance; \
	}

#define PE_IMPLEMENT_SINGLETON_CLASS_ADDON(class_) \
	PE::Handle class_::s_hMyself;


#define PE_IMPLEMENT_CLASS0(class_) \
	PE_IMPLEMENT_CLASS_COMMON(class_) \
	void class_::setSuperClasses(){ \
	for (int i = 0; i < PE_MAX_SUPERCLASSES+1; i++) \
	{ \
	class_::s_metaInfo.m_superClasses[i] = -1; \
	} \
	}

#define PE_IMPLEMENT_SINGLETON_CLASS0(class_) \
	PE_IMPLEMENT_CLASS0(class_) \
	PE_IMPLEMENT_SINGLETON_CLASS_ADDON(class_)


#define PE_IMPLEMENT_CLASS1(class_, superclass1_) \
	PE_IMPLEMENT_CLASS_COMMON(class_) \
	void class_::setSuperClasses(){ \
	for (int i = 1; i < PE_MAX_SUPERCLASSES+1; i++) { \
	class_::s_metaInfo.m_superClasses[i] = -1; \
	}\
	class_::s_metaInfo.m_superClasses[0] = superclass1_::s_metaInfo.m_classId; \
	}

#define PE_IMPLEMENT_SINGLETON_CLASS1(class_, superclass1_) \
	PE_IMPLEMENT_CLASS1(class_, superclass1_) \
	PE_IMPLEMENT_SINGLETON_CLASS_ADDON(class_)

#define PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(method_) \
	static void method_ ## wrapper(PE::Events::Event *pEvt, PE::Handle &h) { h.getObject<MyClass>()->method_(pEvt); }

#if PE_USE_VIRTUAL_EVENT_HANDLERS

#define PE_REGISTER_EVENT_HANDLER(evtClass_, method_) \
	_addMethodToHandlerQueue(evtClass_::GetClassId(), (PE::Components::Component::HandlerMethod)(&method_));
#else
#define PE_REGISTER_EVENT_HANDLER(evtClass_, method_) \
	_addStaticMethodToHandlerQueue(evtClass_::GetClassId(), &method_ ## wrapper);
#endif	
	

// all classes in PyEngine framework are inherited from this class
// it provides some function definitions that are used as default
// for example SetLuaFunctions() is empty and can be overriden by any child class
//
namespace Events
{
	struct Event;
}

struct PEAllocatable
{
	//////////////////////////////////////////////////////////////////////////
	// Custom memory management
	//////////////////////////////////////////////////////////////////////////
	//
	void *operator new(size_t size, MemoryArena arena)
	{
		return pemalloc(arena, size);
	}

	void operator delete(void *ptr, MemoryArena arena)
	{
		pefree(arena, ptr);
	}

	void *operator new[](size_t size, MemoryArena arena)
	{
		return pemalloc(arena, size);
	}

	void operator delete[](void *ptr, MemoryArena arena)
	{
		pefree(arena, ptr);
	}

	void operator delete(void *ptr)
	{
		free(ptr);
		// todo: add check here to make sure that this memory is not handled by a Handle
		// which would mean memory is managed outside and should not be deallocated here
	}
};

struct PEAllocatableAndDefragmentable : public PEAllocatable
{
	//////////////////////////////////////////////////////////////////////////
	// Custom memory management
	//////////////////////////////////////////////////////////////////////////
	//
	void *operator new(size_t size, PE::Handle &h)
	{
		// create this object in allocated memory
		return h.getObject();
	}

	// is called if constructor throws an exception
	void operator delete(void *ptr, PE::Handle &h)
	{
		// don'tdo anything, passed in handle means memory is managed outside
	}

	void *operator new(size_t size, MemoryArena arena)
	{
		return pemalloc(arena, size);
	}

	void operator delete(void *ptr, MemoryArena arena)
	{
		pefree(arena, ptr);
	}

	void operator delete(void *ptr)
	{
		free(ptr);
		// todo: add check here to make sure that this memory is not handled by a Handle
		// which would mean memory is managed outside and should not be deallocated here
	}
};

struct PEClass : PEAllocatableAndDefragmentable
{
	virtual ~PEClass(){}

	// will be overridden if lua functions exist
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM) {}

	static void* FactoryConstruct(PE::GameContext&, PE::MemoryArena) {return NULL;}

	// We need to force virtual method in base classes so that the base class has v-table
	// this is needed for the event triggering mechanism of Component
	virtual void dummy() {}


	virtual int constructFromStream(char *pStream) {assert(!"Not implemented! Need to implement it."); return 0;}
};


}; // namespace PE

#endif
