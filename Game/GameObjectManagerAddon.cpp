#include "GameObjectMangerAddon.h"

#include "PrimeEngine/PrimeEngineIncludes.h"

#include "Characters/SoldierNPC.h"
#include "WayPoint.h"
#include "Tank/ClientTank.h"

using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;
using namespace CharacterControl::Components;

namespace CharacterControl{
namespace Components
{
PE_IMPLEMENT_CLASS1(GameObjectManagerAddon, Component); // creates a static handle and GteInstance*() methods. still need to create construct

void GameObjectManagerAddon::addDefaultComponents()
{
	Component::addDefaultComponents();
}

}
}
