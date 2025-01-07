#ifndef _CharacterControl_GLOBAL_REGISTRY_
#define _CharacterControl_GLOBAL_REGISTRY_

namespace PE
{
	struct GlobalRegistry;
	namespace Components
	{
		struct LuaEnvironment;
	}
}
namespace CharacterControl{
	void Register(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry);
};
#endif
