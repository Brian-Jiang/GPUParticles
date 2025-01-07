#include "SoundManager.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
namespace PE {
namespace Components {
Handle SoundManager::s_hInstance;
bool SoundManager::s_isActive;

PE_IMPLEMENT_CLASS1(SoundManager, Component);

}; // namespace Components
}; // namespace PE