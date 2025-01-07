
#include "ServerCharacterControlGame.h"
#include "GameObjectMangerAddon.h"	
#include "Tank/ClientTank.h"
#include "CharacterControlContext.h"
#include "ServerGameObjectManagerAddon.h"

using namespace PE;
using namespace PE::Components;

namespace CharacterControl {
namespace Components {

// is run after initializing the engine
// basic demo just adds a light source and a default control scheme
// so that uses can add simple objects like meshes, skins, light sources, levels, etc. from asset manager
int ServerCharacterControlGame::initGame()
{
	// super implementation
	ServerGame::initGame();

	//add game specific context
	m_pContext->m_pGameSpecificContext = new (m_arena) CharacterControlContext;

	PE::Components::LuaEnvironment *pLuaEnv = m_pContext->getLuaEnvironment();
	
	// init events, components, and other classes of the project
	CharacterControl::Register(pLuaEnv, PE::GlobalRegistry::Instance());

	// create the GameObjectmanager addon that is in charge of game objects in this demo
	m_pContext->get<CharacterControlContext>()->m_pGameObjectManagerAddon = new(m_arena) CharacterControl::Components::ServerGameObjectManagerAddon(*m_pContext, m_arena, PE::Handle());
	m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon()->addDefaultComponents();
	

	// add it to game object manager
	// now all game events will be passed through to our GameObjectManagerAddon
	m_pContext->getGameObjectManager()->addComponent(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon()->getHandle());


	PE::Handle hroot = RootSceneNode::InstanceHandle();
	RootSceneNode *proot = RootSceneNode::Instance();
	
	return 1; // 1 (true) = success. no errors. TODO: add error checking
}


}
}
