
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "Networkable.h"

#include "PrimeEngine/Networking/NetworkManager.h"

#include "PrimeEngine/Game/Common/GameContext.h"

namespace PE {

Networkable::NetworkId Networkable::s_NetworkId_Invalid;
Networkable::NetworkId Networkable::s_NetworkId_GameObjectManager(1);
Networkable::NetworkId Networkable::s_NetworkId_LuaEnvironment(2);
Networkable::NetworkId Networkable::s_NetworkId_NetworkManager(3);

Networkable::NetworkId Networkable::s_NetworkId_FirstDynamic(1000);

Networkable::Networkable(GameContext &context, void *pMainClassThis, Networkable::NetworkId networkId /* = s_NetworkId_Invalid */)
: m_networkId(networkId)
, m_pMainClassThis(pMainClassThis)
{
	if (m_networkId != s_NetworkId_Invalid)
	{
		registerWithNetwork(context.getNetworkManager());
	}
}

void Networkable::registerWithNetwork(PE::Components::NetworkManager *pNetworkManager)
{
	pNetworkManager->registerNetworkableObject(this);
}

}; // namespace PE

