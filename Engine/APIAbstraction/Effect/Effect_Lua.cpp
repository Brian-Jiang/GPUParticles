#define NOMINMAX

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"


#include "Effect.h"

#include "EffectManager.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {
	namespace Components{
//////////////////////////////////////////////////////////////////////////
// Effect Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void Effect::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Effect[] = {
		{"l_GetTechnique", l_GetTechnique},
		{"l_SetTechnique", l_SetTechnique},
		{"l_SetSpeShaderData", l_SetSpeShaderData},
		{"SetSpeTechniqueReady", l_SetSpeTechniqueReady},

		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_Effect);

	// run a script to add additional functionality to Lua side of Effect
	// that is accessible from Lua
#if APIABSTRACTION_IOS
	pLuaEnv->runScriptWorkspacePath("Code/PrimeEngine/APIAbstraction/Effect/Effect.lua");
#else
	pLuaEnv->runScriptWorkspacePath("Code\\PrimeEngine\\APIAbstraction\\Effect\\Effect.lua");
#endif
}

int Effect::l_GetTechnique(lua_State *luaVM)
{
	GameContext *pContext = (GameContext*)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Effect *pEffect = h.getObject<Effect>();

	lua_newtable(luaVM);

	lua_pushstring(luaVM, "name"); // 
	lua_pushstring(luaVM, pEffect->m_techName);
	lua_rawset(luaVM, -3); // set name

	if (StringOps::length(pEffect->m_vsFilename))
	{
		lua_pushstring(luaVM, "vs"); // 
		lua_newtable(luaVM);

		lua_pushstring(luaVM, "vsname"); // 
		lua_pushstring(luaVM, pEffect->m_vsName);
		lua_rawset(luaVM, -3); // set vsname

		lua_pushstring(luaVM, "filename"); // 
		lua_pushstring(luaVM, pEffect->m_vsFilename);
		lua_rawset(luaVM, -3); // set vsFilename

		lua_pushstring(luaVM, "text"); // 
		char *pdata = 0;
		Effect::readIntoBufferReplaceEOL(*pContext, pdata, pEffect->m_vsFilename);
		lua_pushstring(luaVM, (char *)(pdata));
		delete pdata;
		lua_rawset(luaVM, -3); // set vs text

		lua_rawset(luaVM, -3); // set vs table
	}


	if (StringOps::length(pEffect->m_gsFilename))
	{
		lua_pushstring(luaVM, "gs"); // 
		lua_newtable(luaVM);

		lua_pushstring(luaVM, "gsname"); // 
		lua_pushstring(luaVM, pEffect->m_gsName);
		lua_rawset(luaVM, -3); // set gsname

		lua_pushstring(luaVM, "filename"); // 
		lua_pushstring(luaVM, pEffect->m_gsFilename);
		lua_rawset(luaVM, -3); // set gsFilename

		lua_pushstring(luaVM, "text"); // 
		char *pdata = 0;
		Effect::readIntoBufferReplaceEOL(*pContext, pdata, pEffect->m_gsFilename);
		lua_pushstring(luaVM, (char *)(pdata));
		delete pdata;
		lua_rawset(luaVM, -3); // set gs text

		lua_rawset(luaVM, -3); // set gs table
	}

	if (StringOps::length(pEffect->m_psFilename))
	{
		lua_pushstring(luaVM, "ps"); // 
		lua_newtable(luaVM);

		lua_pushstring(luaVM, "psname"); // 
		lua_pushstring(luaVM, pEffect->m_psName);
		lua_rawset(luaVM, -3); // set vsname

		lua_pushstring(luaVM, "filename"); // 
		lua_pushstring(luaVM, pEffect->m_psFilename);
		lua_rawset(luaVM, -3); // set psFilename

		lua_pushstring(luaVM, "text"); // 
		char *pdata = 0;
		Effect::readIntoBufferReplaceEOL(*pContext, pdata, pEffect->m_psFilename);
		lua_pushstring(luaVM, (char *)(pdata));
		delete pdata;
		lua_rawset(luaVM, -3); // set ps text

		lua_rawset(luaVM, -3); // set ps table
	}

	if (StringOps::length(pEffect->m_csFilename))
	{
		lua_pushstring(luaVM, "cs"); // 
		lua_newtable(luaVM);

		lua_pushstring(luaVM, "csname"); // 
		lua_pushstring(luaVM, pEffect->m_csName);
		lua_rawset(luaVM, -3); // 

		lua_pushstring(luaVM, "filename"); // 
		lua_pushstring(luaVM, pEffect->m_csFilename);
		lua_rawset(luaVM, -3); // 

		lua_pushstring(luaVM, "text"); // 
		char *pdata = 0;
		Effect::readIntoBufferReplaceEOL(*pContext, pdata, pEffect->m_csFilename);
		lua_pushstring(luaVM, (char *)(pdata));
		delete pdata;
		lua_rawset(luaVM, -3); // set cs text

		lua_rawset(luaVM, -3); // set cs table
	}

	if (StringOps::length(pEffect->m_spesFilename))
	{
		lua_pushstring(luaVM, "spes"); // 
		lua_newtable(luaVM);

		lua_pushstring(luaVM, "spesname"); // 
		lua_pushstring(luaVM, pEffect->m_spesName);
		lua_rawset(luaVM, -3); // 

		lua_pushstring(luaVM, "filename"); // 
		lua_pushstring(luaVM, pEffect->m_spesFilename);
		lua_rawset(luaVM, -3); // 

		lua_pushstring(luaVM, "text"); // 
		char *pdata = 0;
		Effect::readIntoBufferReplaceEOL(*pContext, pdata, pEffect->m_spesFilename);
		lua_pushstring(luaVM, (char *)(pdata));
		delete pdata;
		lua_rawset(luaVM, -3); // set cs text

		lua_rawset(luaVM, -3); // set cs table
	}

	return 1;
}

int Effect::l_SetTechnique(lua_State *luaVM)
{
	GameContext *pContext = (GameContext*)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

	Handle h;
	char filename[128];


	lua_pushstring(luaVM, "vs"); // vertex shader
	lua_gettable(luaVM, -2);

	if (lua_istable(luaVM,-1))
	{
		lua_pushstring(luaVM, "filename"); // filename
		lua_gettable(luaVM, -2);
		const char *pfilename = lua_tostring(luaVM, -1);
		StringOps::writeToString(pfilename, filename, 128);
		lua_pop(luaVM, 1); // pop vs filename

		lua_pushstring(luaVM, "text"); // text
		lua_gettable(luaVM, -2);
		const char *ptext = lua_tostring(luaVM, -1);
		{
			Effect::writeEffectFile(*pContext, filename, ptext);
		}
		lua_pop(luaVM, 1); // pop vs text
	}
	lua_pop(luaVM, 1); // pop vs


	lua_pushstring(luaVM, "gs"); // geometry shader
	lua_gettable(luaVM, -2);

	if (lua_istable(luaVM,-1))
	{
		lua_pushstring(luaVM, "filename"); // filename
		lua_gettable(luaVM, -2);
		const char *pfilename = lua_tostring(luaVM, -1);
		StringOps::writeToString(pfilename, filename, 128);
		lua_pop(luaVM, 1); // pop gs filename

		lua_pushstring(luaVM, "text"); // text
		lua_gettable(luaVM, -2);
		const char *ptext = lua_tostring(luaVM, -1);
		{
			Effect::writeEffectFile(*pContext, filename, ptext);
		}
		lua_pop(luaVM, 1); // pop gs text
	}
	lua_pop(luaVM, 1); // pop gs

	lua_pushstring(luaVM, "ps"); // pixel shader
	lua_gettable(luaVM, -2);

	if (lua_istable(luaVM,-1))
	{
		lua_pushstring(luaVM, "filename"); // filename
		lua_gettable(luaVM, -2);
		const char *pfilename = lua_tostring(luaVM, -1);
		StringOps::writeToString(pfilename, filename, 128);
		lua_pop(luaVM, 1); // pop ps filename

		lua_pushstring(luaVM, "text"); // text
		lua_gettable(luaVM, -2);
		const char *ptext = lua_tostring(luaVM, -1);
		{
			Effect::writeEffectFile(*pContext, filename, ptext);
		}
		lua_pop(luaVM, 1); // pop ps text
	}
	lua_pop(luaVM, 1); // pop ps

	lua_pushstring(luaVM, "cs"); // vertex shader
	lua_gettable(luaVM, -2);

	if (lua_istable(luaVM,-1))
	{
		lua_pushstring(luaVM, "filename"); // filename
		lua_gettable(luaVM, -2);
		const char *pfilename = lua_tostring(luaVM, -1);
		StringOps::writeToString(pfilename, filename, 128);
		lua_pop(luaVM, 1); // pop cs filename

		lua_pushstring(luaVM, "text"); // text
		lua_gettable(luaVM, -2);
		const char *ptext = lua_tostring(luaVM, -1);
		{
			Effect::writeEffectFile(*pContext, filename, ptext);
		}
		lua_pop(luaVM, 1); // pop cs text
	}
	lua_pop(luaVM, 1); // pop cs


	lua_pushstring(luaVM, "spes"); // spe shader
	lua_gettable(luaVM, -2);

	if (lua_istable(luaVM,-1))
	{
		lua_pushstring(luaVM, "filename"); // filename
		lua_gettable(luaVM, -2);
		const char *pfilename = lua_tostring(luaVM, -1);
		StringOps::writeToString(pfilename, filename, 128);
		lua_pop(luaVM, 1); // pop cs filename

		lua_pushstring(luaVM, "text"); // text
		lua_gettable(luaVM, -2);
		const char *ptext = lua_tostring(luaVM, -1);
		{
			Effect::writeEffectFile(*pContext, filename, ptext);
		}
		lua_pop(luaVM, 1); // pop spes text
	}
	lua_pop(luaVM, 1); // pop spes

	lua_pop(luaVM, 1); // pop input table

	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Effect *pEffect = h.getObject<Effect>();

	pEffect->loadTechniqueAsync();

	return 1;
}


int Effect::l_SetSpeShaderData(lua_State *luaVM)
{
	GameContext *pContext = (GameContext*)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

	PrimitiveTypes::UInt32 size;

	lua_pushstring(luaVM, "size"); // 
	lua_gettable(luaVM, -2);
	size = (PrimitiveTypes::UInt32)(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1); // pop size


	Array<float> vals(*pContext, MemoryArena_Client);
	vals.reset(size * (3+2));
	float *pcur = vals.getFirstPtr();

	int ival = 1; // start indices from 1
	for (PrimitiveTypes::UInt32 i = 0; i < size; ++i)
	{
		for (PrimitiveTypes::UInt32 j = 0; j < 3; ++j)
		{
			lua_pushnumber(luaVM, ival++); 
			lua_gettable(luaVM, -2); // 

			PrimitiveTypes::Float32 val = (PrimitiveTypes::Float32)(lua_tonumber(luaVM, -1));
			*pcur = val;
			pcur++;
			lua_pop(luaVM, 1); // pop value
		}

		*pcur = 100.0f;
		pcur++;
		*pcur = 100.0f;
		pcur++;
	}


	lua_pushstring(luaVM, "techName"); // 
	lua_gettable(luaVM, -2);
	const char * techName = lua_tostring(luaVM, -1);
	lua_pop(luaVM, 1); // pop techName

	Handle h = EffectManager::Instance()->getEffectHandle(techName);

	if (h.isValid())
	{
		Effect *pEffect = h.getObject<Effect>();
		void * dest = pEffect->m_speData.getObject();
		memcpy(dest, vals.getFirstPtr(), size * (3+2) * sizeof(float));
	}
	else
	{
		PEINFO("PE: ERROR: l_setSpeTechniqueReady() : provided technique does not exist!");
	}

	lua_pop(luaVM, 1); // pop the input table

	vals.reset(0);

	return 0;
}


int Effect::l_SetSpeTechniqueReady(lua_State *luaVM)
{
#if APIABSTRACTION_D3D11
	const char *techName = lua_tostring(luaVM, -1);

	Handle h = EffectManager::Instance()->getEffectHandle(techName);
	if (h.isValid())
	{
		Effect *pEffect = h.getObject<Effect>();
		pEffect->setSpeShaderSatus(true);
	}
	else
	{
		PEINFO("PE: ERROR: l_setSpeTechniqueReady() : provided technique does not exist!");
	}

	lua_pop(luaVM, 1);
#else
	assert(!"Not Implemented");
#endif
	return 0;
}

}; // namespace Components
}; // namespace PE
