#include "LuaEnvironment.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/Events/StandardGameEvents.h"
#include "../Logging/Log.h"
#include "EventGlue/EventDataCreators.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/GameObjectModel/GameObjectManager.h"

#include "../../../GlobalConfig/GlobalConfig.h"

#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
#include <io.h>
#elif APIABSTRACTION_PS3 // ps3 current folder printouts for debugging
#include <dirent.h>
#endif


namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(LuaEnvironment, Component);

Handle LuaEnvironment::s_myHandle;
#if APIABSTRACTION_PS3
int getdir (const char *dir)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir)) == NULL) {
        printf("Error(%d) opening %s\n", errno, dir);
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);
    return 0;
}
#endif

// Constructor -------------------------------------------------------------
// note we cant pass in lua environment network id here because network manager is not constructed when
// this class is created. it will be set later
LuaEnvironment::LuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, Networkable(context, this) // cant register with networkable here because LueEnvironemntr is created before NetworkManager
{

#if APIABSTRACTION_PS3
	getdir("/app_home/");
#endif

	L = luaL_newstate(); // calls lua_newstate with standard C realloc

	m_luaStates[LuaThread_Game] = L;
	m_luaStates[LuaThread_Physics] = lua_newthread(L);
	m_luaStates[LuaThread_Draw] = lua_newthread(L);
	m_luaStates[LuaThread_Network] = lua_newthread(L);	
}

void LuaEnvironment::closeStates()
{
	for (int i = 0; i < LuaThread_Count; ++i)
	{
		lua_close(m_luaStates[i]);
	}
}

void LuaEnvironment::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, LuaEnvironment::do_UPDATE);
}

void LuaEnvironment::do_UPDATE(Events::Event *pEvt)
{
	prepFunctionCall("handleEvent");
	LuaGlue::pushTableBuiltFromEvent(L, pEvt);
	callPreparedFunction(1, 0, 0);
}

void LuaEnvironment::pop()
{
	lua_pop(L, 1);
}

void LuaEnvironment::stringToHandle(lua_State* luaVM, const char *pString)
{
	int x = lua_gettop(luaVM);
	bool b = lua_istable(luaVM, -1);
	int a = lua_type(luaVM, -1);

	prepFunctionCall("stringAsHandle");
	LuaGlue::pushString(luaVM, pString);
	callPreparedFunction(1, 1, 0);

	x = lua_gettop(luaVM);
	b = lua_istable(luaVM, -1);
	a = lua_type(luaVM, -1);
}

void LuaEnvironment::pushString(const char *pString)
{
	lua_pushstring(L, pString);
}

void LuaEnvironment::pushNumber(PrimitiveTypes::Double value)
{
	lua_pushnumber(L, value);
}

void LuaEnvironment::pushPEUUID(const PEUUID &peuuid)
{
	LuaGlue::pushPEUUID(L, peuuid);
}

void LuaEnvironment::pushInt32(PrimitiveTypes::Int32 v, lua_State *pL)
{
	LuaGlue::pushInt32(pL, v);
}

void LuaEnvironment::pushTableBuiltFromEvent(Events::Event *pEvt)
{
	LuaGlue::pushTableBuiltFromEvent(L, pEvt);
}

void LuaEnvironment::pushNewTableAsField(PEUUID &key)
{
	assert(lua_istable(L, -1));
	pushPEUUID(key);
	LuaGlue::createTableOnTopOfStack(L);
	lua_rawset(L, -3);

	// the parent table is on top of the stack
}

void LuaEnvironment::pushNewTableAsFieldKeyedByInt32(PrimitiveTypes::Int32 key)
{
	assert(lua_istable(L, -1));
	pushInt32(key,L);
	LuaGlue::createTableOnTopOfStack(L);
	lua_rawset(L, -3);
	// the parent table is on top of the stack
}

void LuaEnvironment::pushNewTableAsFieldKeyedByString(const char * key, bool leaveOnTop)
{
	assert(lua_istable(L, -1));
	LuaGlue::pushString(L, key);
	LuaGlue::createTableOnTopOfStack(L);
	lua_rawset(L, -3);
	if (leaveOnTop)
	{
		lua_getfield(L, -1, key);
		assert(lua_istable(L, -1));
	}
}

void LuaEnvironment::pushHandle(Handle &h)
{
	LuaGlue::pushTableBuiltFromHandle(L, h);
}

void LuaEnvironment::pushHandleAsFieldAndSet(PEUUID &key, Handle &h)
{
	pushPEUUID(key);
	LuaGlue::pushTableBuiltFromHandle(L, h);
	lua_rawset(L, -3);
}

bool LuaEnvironment::pushHandleAsNextArrayElementIfNotInArray(lua_State *L, Handle &h)
{
	PrimitiveTypes::UInt32 size = 0;
	// a queue for an event exists and is on top

	 /* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0)
	{
		size++;
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",
		//	lua_typename(L, lua_type(L, -2)),
		//	lua_typename(L, lua_type(L, -1)));

		if (lua_istable(L, -1))
		{
			// value on top is a table that stores handle
			Handle th;
			/* removes 'value'; keeps 'key' for next iteration */
			LuaEnvironment::popHandleFromTableOnStackAndPopTable(L, th);
			// check if this handle is smae as one we are adding
			if (th == h)
			{
				lua_pop(L, 1);
				return false; // this handle is already in the list}
			}
		}
		else
		{
			lua_pop(L, 1);
		}
	}

	// this handle is not in the list. add it to list
	
	LuaGlue::pushNumber(L, (double)(size + 1));
	LuaGlue::pushTableBuiltFromHandle(L, h);
	lua_rawset(L, -3);
	return true; //added
}

bool LuaEnvironment::pushMethodAsNextArrayElementIfNotInArray(lua_State *L, Component::HandlerMethod method)
{
	PrimitiveTypes::UInt32 size = 0;
	// a queue for an event exists and is on top

	 /* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0)
	{
		size++;
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",
		//	lua_typename(L, lua_type(L, -2)),
		//	lua_typename(L, lua_type(L, -1)));

		if (!lua_istable(L, -1))
		{
			Component::HandlerMethod imethod;
			void *ptr = lua_touserdata(L, -1);
			memcpy(&imethod, &ptr, sizeof(Component::HandlerMethod));
			lua_pop(L, 1);
			if (imethod == method)
			{
				lua_pop(L, 1);
				return false; // thsi method is already in the table
			}
		}
		else
		{
			// skip tables (handles)
			lua_pop(L, 1);
		}
		
	}

	// this method is not in the list. add it to list
	
	LuaGlue::pushNumber(L, (double)(size + 1));
	LuaGlue::pushMethodLightUserData(L, method);
	lua_rawset(L, -3);
	return true; //added
}


bool LuaEnvironment::pushStaticMethodAsNextArrayElementIfNotInArray(lua_State *L, Component::StaticHandlerMethod method)
{
	PrimitiveTypes::UInt32 size = 0;
	// a queue for an event exists and is on top

	 /* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0)
	{
		size++;
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",
		//	lua_typename(L, lua_type(L, -2)),
		//	lua_typename(L, lua_type(L, -1)));

		if (!lua_istable(L, -1))
		{
			Component::StaticHandlerMethod imethod;
			void *ptr = lua_touserdata(L, -1);
			memcpy(&imethod, &ptr, sizeof(Component::StaticHandlerMethod));
			lua_pop(L, 1);
			if ((imethod == method) || PE_DONT_ALLOW_MULTIPLE_METHODS_IN_EVENT_PROCESSING_QUEUE)
			{
				lua_pop(L, 1);
				return false; // this method is already in the table
			}
		}
		else
		{
			// skip tables (handles)
			lua_pop(L, 1);
		}
		
	}

	// this method is not in the list. add it to list
	
	LuaGlue::pushNumber(L, (double)(size + 1));
	LuaGlue::pushStaticMethodLightUserData(L, method);
	lua_rawset(L, -3);
	return true; //added
}

void LuaEnvironment::popHandleFromTableOnStackAndPopTable(lua_State *L, Handle &h)
{
	LuaGlue::popHandleFromTableOnStackAndPopTable(L, h);
}

LuaGlue::LuaReference LuaEnvironment::createTableOnTopOfStackAndStoreReference(lua_State *pL)
{
	LuaGlue::createTableOnTopOfStack(pL);
	return LuaGlue::storeReferenceToTop(pL);
}

void LuaEnvironment::putTableOnTopOfStackByReference(LuaGlue::LuaReference ref, lua_State *pL)
{
	LuaGlue::putTableOnTopOfStackByReference(pL, ref);
}

PrimitiveTypes::Double LuaEnvironment::popNumber()
{
	PrimitiveTypes::Double n =  lua_tonumber(L, -1);
	lua_pop(L, 1);
	return n;
}

bool LuaEnvironment::checkTableValueByPEUUIDFieldExists(PEUUID &key)
{
	pushPEUUID(key);
	lua_gettable(L, -2);
	return !lua_isnil(L, -1);
}

bool LuaEnvironment::checkTableValueByInt32KeyExists(int key)
{
	pushInt32(key, L);
	lua_gettable(L, -2);
	return !lua_isnil(L, -1);
}

// sets a value for a table currently on stack
void LuaEnvironment::setTableValue(const  char *key, double val, lua_State *pL)
{
	if (pL)
	{
		lua_pushnumber(pL, val);
		lua_setfield(pL, -2, key);
	}
	else
	{
		lua_pushnumber(L, val);
		lua_setfield(L, -2, key);
	}
}
void LuaEnvironment::pushInt32AsFieldAndSet(const char*key, PrimitiveTypes::Int32 val, lua_State *pL)
{
	if (pL)
	{
		pushInt32(val, pL);
		lua_setfield(pL, -2, key);
	}
	else
	{
		pushInt32(val, L);
		lua_setfield(L, -2, key);
	}
}


int LuaEnvironment::l_getClassNameByClassId(lua_State *luaVM)
{
	PrimitiveTypes::Int32 classId = LuaGlue::readInt32(luaVM, -1);
	
	const char* className = GlobalRegistry::Instance()->getMetaInfo(classId)->getClassName();

	lua_pushstring(luaVM, className);
	return 1;
}


void LuaEnvironment::passEventToQueue(Events::Event *pEvt)
{
	// a queue for this event exists and is on top

	 /* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0)
	{
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",
		//	lua_typename(L, lua_type(L, -2)),
		//	lua_typename(L, lua_type(L, -1)));

		assert(!lua_isnil(L, -1));
		// value on top is a table that stores handle or a pointer
		Handle h;
		/* removes 'value'; keeps 'key' for next iteration */
		if (lua_istable(L, -1))
		{
			LuaEnvironment::popHandleFromTableOnStackAndPopTable(L, h);
			
			h.getObject<Component>()->handleEvent(pEvt);
		}
		else
		{
			// is a pointer to method
			void *ptr = lua_touserdata(L, -1);
			//char buf[80];
			//sprintf(buf, "sizeof(void *) = %d; sizeof(Component::HandlerMethod) = %d\n", sizeof(void *), sizeof(Component::HandlerMethod));
			//PEINFOSTR(buf);

			Handle cachedDistributor = pEvt->m_lastDistributor;
			
#if PE_USE_VIRTUAL_EVENT_HANDLERS
			// Use pointers to methods that are then mapped to virtual methods using RTTI
			Component::HandlerMethod imethod;
			memcpy(&imethod, &ptr, sizeof(Component::HandlerMethod));

			(pEvt->m_lastDistributor.getObject<Component>()->*imethod)(pEvt);
#else
			// use pointers to static functions that wrap method calls
			Component::StaticHandlerMethod imethod;
			#if PE_PLAT_IS_PSVITA
				int s = 4; // for some reson vita doesnt resolve size of function
			#else
				int s = sizeof(void *); // sizeof(Component::HandlerMethod); does not always produce sizeof(void*) on ios at least
			#endif
			memcpy(&imethod, &ptr, s);

			imethod(pEvt, pEvt->m_lastDistributor);
			//(pEvt->m_lastDistributor.getObject<Component>()->*imethod)(pEvt);

#endif
			pEvt->m_lastDistributor = cachedDistributor;
			
			lua_pop(L, 1);
		}
		if (pEvt->m_cancelSiblingAndChildEventHandling)
		{
			pEvt->m_cancelSiblingAndChildEventHandling = false; // restore to default as the event goes back to parents
			lua_pop(L, 1);
			break;
		}
	}
}

void LuaEnvironment::iterateOverTableAndGetEventTypes(Array<PrimitiveTypes::Int32> &arr)
{
	arr.reset(512);

	// a table of queues exists on top

	 /* table is in the stack at index 't' */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0)
	{
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",
		//	lua_typename(L, lua_type(L, -2)),
		//	lua_typename(L, lua_type(L, -1)));

		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 1);
		PrimitiveTypes::Int32 evtClassId = LuaGlue::readInt32(L, -1); // read peeuid of the event
		arr.add(evtClassId);
	}
}

PrimitiveTypes::Bool LuaEnvironment::runString(const char *pCommand)
{ 
	PrimitiveTypes::Int32 errCode;
	errCode = luaL_loadbuffer(L, pCommand, strlen(pCommand), NULL);
	if(!errCode)
	{
		if(0 == lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			return true;
		}
	}
	
	return false;
}


PrimitiveTypes::Bool LuaEnvironment::runScript(const char *pFname)
{
	PEINFO("PROGRESS: LuaEnvironment::runScript: %s\n", pFname);
	
	PrimitiveTypes::Int32 errCode;
	errCode = luaL_loadfile(L, pFname);

	if(!errCode)
	{
		if(0 == lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			return true;
		}
		else
		{
			PEINFO("Error while running script in lua VM. Stack: \n%s\n", lua_tostring(L, -1));
			assert(!"Error while running script in lua VM");
		}
	}
	else
	{
		if (errCode == LUA_ERRSYNTAX)
		{
			PEINFO("Error loading lua script: Syntax Error\n%s\n", lua_tostring(L, -1));
			assert(!"Error loading lua script: Syntax Error");
		}
		else
		{
			PEINFO("Error loading lua script\n%s\n", lua_tostring(L, -1));
			assert(!"Error while running script in lua VM");
		}
	}
	

	return false;
}

PrimitiveTypes::Bool LuaEnvironment::runScriptDefaultPath(const char *pFname)
{
	char fullPath[512];
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
    StringOps::concat(m_pContext->getMainFunctionArgs()->gameProjRoot(), "Code/PrimeEngine/Lua/", fullPath, 512);
#else
	StringOps::concat(m_pContext->getMainFunctionArgs()->gameProjRoot(), "Code\\PrimeEngine\\Lua\\", fullPath, 512);
#endif
	StringOps::concat(fullPath, pFname, fullPath, 512);

    return runScript(fullPath);
}

PrimitiveTypes::Bool LuaEnvironment::runScriptWorkspacePath(const char *pFname)
{
	char fullPath[512];
	StringOps::concat(m_pContext->getMainFunctionArgs()->gameProjRoot(), pFname, fullPath, 512);
	return runScript(fullPath);
}



PrimitiveTypes::Int32 LuaEnvironment::prepFunctionCall(const char *fname)
{
	lua_getglobal(L, fname);
	if (!lua_isfunction(L, -1))
	{
		LuaGlue::debugStackObject(L, -1);
		PEASSERT(lua_isfunction(L, -1), "Error looking up function %s", fname);
	}
	return 0;
}

PrimitiveTypes::Int32 LuaEnvironment::prepModuleFunctionCall(const char *module, const char *fname)
{
	lua_getglobal(L, module);
	PEASSERT(lua_istable(L, -1), "Error looking up module %s", module);
	
	lua_pushstring(L, fname);
	lua_gettable(L, -2);

	if (!lua_isfunction(L, -1))
	{
		LuaGlue::debugStackObject(L, -1);
		PEASSERT(lua_isfunction(L, -1), "Error looking up function %s", fname);
	}

	lua_remove(L, -2); // remove module table, and leave only function

	return 0;
}


PrimitiveTypes::Int32 LuaEnvironment::callPreparedFunction(int nargs, int nresults, int errfunc)
{
	int x = lua_pcall(L, nargs, nresults, 0);
	if (x != errfunc)
	{
		PEASSERT(false, "Error while running function in lua VM");
	}
	return 0;
}

PrimitiveTypes::Int32 LuaEnvironment::runFunction(const char *fname, int nresults, int errfunc)
{
	lua_getglobal(L, fname);
	if (lua_pcall(L, 0, nresults, 0) != errfunc)
	{
		PEASSERT(false, "Error while running function in lua VM");
	}
	return 0;
}


int LuaEnvironment::lua_getGameObjectManagerHandle(lua_State* luaVM)
{
	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

	LuaGlue::pushTableBuiltFromHandle(luaVM, Handle(pContext->getGameObjectManager()));
	return 1;
}

int LuaEnvironment::l_getLastAddedGameObjectHandle(lua_State* luaVM)
{
	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

    LuaGlue::pushTableBuiltFromHandle(luaVM, pContext->getGameObjectManager()->m_lastAddedObjHandle);
	return 1;
}

int LuaEnvironment::l_getEffectManagerHandle(lua_State* luaVM)
{
	LuaGlue::pushTableBuiltFromHandle(luaVM, EffectManager::InstanceHandle());
	return 1;
}

int LuaEnvironment::lua_getRootSceneNodeHandle(lua_State* luaVM)
{
	LuaGlue::pushTableBuiltFromHandle(luaVM, RootSceneNode::InstanceHandle());
	return 1;
}

void LuaEnvironment::registerInitialLibrariesFunctions()
{
	luaL_openlibs(L);
	// general purpose lua glue function
	LuaGlue::registerFunctions(L);
	luaopen_socket_core(L);

	lua_register(L, "getGameObjectManagerHandle", lua_getGameObjectManagerHandle);
	lua_register(L, "l_getLastAddedGameObjectHandle", l_getLastAddedGameObjectHandle);
	
	lua_register(L, "l_getEffectManagerHandle", l_getEffectManagerHandle);
	lua_register(L, "l_getRootSceneNodeHandle", lua_getRootSceneNodeHandle);

	lua_register(L, "l_findluaFiles", l_findluaFiles);
	lua_register(L, "l_getClassNameByClassId", l_getClassNameByClassId);
	lua_register(L, "l_getGameProjRoot", l_getGameProjRoot);
	lua_register(L, "l_getLuaCommandServerPort", l_getLuaCommandServerPort);
	lua_register(L, "l_setCreatedLuaCommandServerPort", l_setCreatedLuaCommandServerPort);
	
	lua_register(L, "l_getPathDelimeter", l_getPathDelimeter);
	lua_register(L, "l_convertUserdataPointerToNumber", l_convertUserdataPointerToNumber);
}

int LuaEnvironment::l_getGameProjRoot(lua_State* luaVM)
{
	PE::GameContext *pContext = (PE::GameContext *)(lua_touserdata(luaVM, -1));
	
	lua_pop(luaVM, 1);
	lua_pushstring(luaVM, pContext->getMainFunctionArgs()->gameProjRoot());
	return 1;
}

int LuaEnvironment::l_getLuaCommandServerPort(lua_State* luaVM)
{
	PE::GameContext *pContext = (PE::GameContext *)(lua_touserdata(luaVM, -1));

	lua_pop(luaVM, 1);
	lua_pushnumber(luaVM, (lua_Number)(pContext->getLuaCommandServerPort()));
	return 1;
}

int LuaEnvironment::l_setCreatedLuaCommandServerPort(lua_State* luaVM)
{
	PE::GameContext *pContext = (PE::GameContext *)(lua_touserdata(luaVM, -2));
	
	int port = (int)(lua_tonumber(luaVM, -1));

	lua_pop(luaVM, 2);
	pContext->m_luaCommandServerPort = (unsigned short)(port);

	return 0;
}
int LuaEnvironment::l_getPathDelimeter(lua_State* luaVM)
{
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
    lua_pushstring(luaVM, "/");
#else
    lua_pushstring(luaVM, "\\");
#endif
	return 1;
}
int LuaEnvironment::l_convertUserdataPointerToNumber(lua_State *luaVM)
{
	void *ptr = lua_touserdata(luaVM, -1);
	lua_pop(luaVM, 1);

	double val = (double)((int)(ptr));
	lua_pushnumber(luaVM, val);
	return 1;
}


int LuaEnvironment::l_findluaFiles(lua_State* luaVM)
{
	PE::GameContext *pContext = (PE::GameContext *)(lua_touserdata(luaVM, -2));

	const char* pattern = lua_tostring(luaVM, -1);
	PEString spattern(*pContext, pContext->getDefaultMemoryArena());
	spattern.append(pattern);
	
	lua_pop(luaVM, 2);
	
	// result
	lua_newtable(luaVM);
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS
	PEString path(*pContext, pContext->getDefaultMemoryArena());
	path.append(pContext->getMainFunctionArgs()->gameProjRoot()); // this will get to PyEngienWorkspace (PEW)
	
	PrimitiveTypes::UInt32 curIndex = 1;
	findluaFilesRecursive(*pContext, pContext->getDefaultMemoryArena(), path, spattern, luaVM, curIndex);
	path.release();
	spattern.release();
#endif
	return 1;
}

void LuaEnvironment::findluaFilesRecursive(PE::GameContext &context, PE::MemoryArena arena, PEString &path, PEString &pattern, lua_State* luaVM, PrimitiveTypes::UInt32 &curIndex)
{
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	PEString pathTmp(context, arena);
	pathTmp.append(path);
	pathTmp.append(pattern);

	struct _finddata_t c_file;
	long hFile;

	// first look for *.luah files
	if ( (hFile = _findfirst(pathTmp.m_data.getFirstPtr(), &c_file)) != -1L )
	{
		do
		{
			PEString t(context, arena);
			t.append(path);
			t.append(c_file.name);

			lua_pushnumber(luaVM, curIndex++); // index
			lua_pushstring(luaVM, t.m_data.getFirstPtr()); // value
			lua_rawset(luaVM, -3);

			t.release();

		} while ( _findnext(hFile, &c_file) == 0 );
		_findclose(hFile);
	}

	// parse sub folders
	pathTmp.decreaseSize(pattern.m_data.m_size-1-1); // strip ".luah"

	struct _finddata_t dir_file;
	
	if ( (hFile = _findfirst(pathTmp.m_data.getFirstPtr(), &dir_file)) != -1L )
	{
		do
		{
			if (StringOps::strcmp(dir_file.name, ".") != 0 &&
				StringOps::strcmp(dir_file.name, "..") != 0 &&
				StringOps::strcmp(dir_file.name, ".svn"))
			{
				PEString t(context, arena);
				t.append(path);
				t.append(dir_file.name);

				if (GetFileAttributesA(t.m_data.getFirstPtr()) & FILE_ATTRIBUTE_DIRECTORY)
				{
					t.append("\\");
					findluaFilesRecursive(context, arena, t, pattern, luaVM, curIndex);
				}
				t.release();
			}
		} while ( _findnext(hFile, &dir_file) == 0 );
		_findclose(hFile);
	}
	pathTmp.release();
#endif
}


//////////////////////////////////////////////////////////////////////////
// LuaEnvironment Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void LuaEnvironment::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_LuaEnvironment[] = {
		{"l_MemoryReport", l_MemoryReport},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_LuaEnvironment);
}
//
int LuaEnvironment::l_MemoryReport(lua_State* luaVM)
{
	Handle hReportStr("RAW_DATA", 2048);
	PrimitiveTypes::UInt32 size = 2048;
	MemoryManager::instance()->memoryReport(hReportStr.getObject(), size);
	char *s = hReportStr.getObject<char>();

	lua_pushstring(luaVM, s);

	hReportStr.release();

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// PE class registration utilities
//////////////////////////////////////////////////////////////////////////
//
void LuaEnvironment::StartRootRegistrationTable()
{
	lua_getglobal(L, "root");
	bool res = lua_istable(L, -1);
    assert(res);
}
//
void LuaEnvironment::EndRootRegistrationTable()
{
	bool res = lua_istable(L,-1); // check root is still in top
	assert(res);
    lua_pop(L, 1);
}//
void LuaEnvironment::StartRegistrationTable(const char *tableName)
{
	lua_pushstring(L, tableName);
	lua_newtable(L); // will be set as tableName
	lua_settable(L, -3); // created tableName
	// get tableName back on top
	lua_getfield(L, -1, tableName);
	bool res = lua_istable(L,-1); // check that tableName is a table
    assert(res);
}
//
void LuaEnvironment::EndRegistrationTable()
{
	bool res = lua_istable(L,-1); // check table is still in top
    assert(res);
	lua_pop(L, 1);
}
//

}; // namespace Components
}; // namespace PE


