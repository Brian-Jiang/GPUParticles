
#include "ClientBasicGame.h"

using namespace PE::Components;

namespace Basic {
namespace Components {

PE_IMPLEMENT_CLASS1(TankController, Component);
    
TankController::TankController(PE::GameContext &context, PE::MemoryArena arena, PE::Handle myHandle, float speed)
: Component(context, arena, myHandle)
, m_timeSpeed(speed)
, m_time(0)
, m_counter(0)
{}
    
void TankController::addDefaultComponents()
{
    Component::addDefaultComponents();
        
    PE_REGISTER_EVENT_HANDLER(PE::Events::Event_UPDATE, TankController::do_UPDATE);
}
    
void TankController::do_UPDATE(PE::Events::Event *pEvt)
{
    m_time += m_timeSpeed;
    
    if (m_time > 2.0f*PrimitiveTypes::Constants::c_Pi_F32)
    {
        m_time = 0;
        if (m_counter)
        {
            m_counter = 0;
            m_center = Vector2(0,0);
        }
        else
        {
            m_counter = 1;
            m_center = Vector2(1000.0f, 0);
        }
    }
    
    Vector3 pos = Vector3(m_center.m_x, 0, m_center.m_y);
    pos.m_x += (float)cos(m_time) * 500.0f * (m_counter ? -1.0f : 1.0f);
    pos.m_z += (float)sin(m_time) * 500.0f;
    pos.m_y = 0;
    
    Vector3 fwrd;
    fwrd.m_x = -(float)sin(m_time)  * (m_counter ? -1.0f : 1.0f);
    fwrd.m_z = (float)cos(m_time);
    fwrd.m_y = 0;
    
    Vector3 right;
    right.m_x = (float)cos(m_time)  * (m_counter ? -1.0f : 1.0f);
    right.m_z = (float)sin(m_time);
    right.m_y = 0;
    
    PE::Handle hFisrtSN =  getFirstComponentHandle<SceneNode>();
    if (hFisrtSN.isValid())
    {
        SceneNode *pFirstSN = hFisrtSN.getObject<SceneNode>();
            
        pFirstSN->m_base.setPos(pos);
        pFirstSN->m_base.setN(fwrd);
        pFirstSN->m_base.setU(right);
    }
    
} 

// is run after initializing the engine
// basic demo just adds a light source and a default control scheme
// so that uses can add simple objects like meshes, skins, light sources, levels, etc. from asset manager
int ClientBasicGame::initGame()
{
	// super implementation
	ClientGame::initGame();

	PE::Components::LuaEnvironment *pLuaEnv = m_pContext->getLuaEnvironment();
	
	// init events, components, and other classes of the project
	Basic::Register(pLuaEnv, PE::GlobalRegistry::Instance());


	// grey-ish background
	PE::GPUScreen::Instance()->setClearColor(Vector4(0.1f, 0.1f, 0.1f, 0.0f));

	// game controls read input queue and post events onto general queue
	// the events from general queue are then passed on to game components
    PE::Handle hDefaultGameControls("GAME_CONTROLS", sizeof(DefaultGameControls));
	DefaultGameControls *pDefaultGameControls = new(hDefaultGameControls) DefaultGameControls(*m_pContext, m_arena, hDefaultGameControls);
	pDefaultGameControls->addDefaultComponents();

	m_pContext->getGameObjectManager()->addComponent(hDefaultGameControls);


    PE::Handle hroot = RootSceneNode::InstanceHandle();

	//create text
	{
		// need to create a scene node for this mesh
		PE::Handle hSN("SCENE_NODE", sizeof(TextSceneNode));
		TextSceneNode *pSN = new(hSN) TextSceneNode(*m_pContext, m_arena, hSN);
		pSN->addDefaultComponents();

		pSN->loadFromString_needsRC("Basic Game", TextSceneNode::Overlay2D, m_pContext->m_gameThreadThreadOwnershipMask);
		
		RootSceneNode::Instance()->addComponent(hSN);
	}
	


    //create hierarchy:
    //scene root
    //  scene node // tracks position/orientation
    //    Tank
    
    //game object manager
    //  TankController
    //    scene node
	if (false) // set to true to add a tank
    {
		PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
		MeshInstance *pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);

		pMeshInstance->initFromFile("nazicar.x_carmesh_mesh.mesha", "Default", m_pContext->m_gameThreadThreadOwnershipMask);
		pMeshInstance->addDefaultComponents();

        
		// need to create a scene node for this mesh
        PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
        SceneNode *pSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
        pSN->addDefaultComponents();
            
        pSN->addComponent(hMeshInstance);
            
        RootSceneNode::Instance()->addComponent(hSN);
		
        // now add game objects
        PE::Handle hSquareController("DbgSquareController0", sizeof(TankController));
        TankController *pSquareController = new(hSquareController) TankController(*m_pContext, m_arena, hSquareController, 0.05f);
        pSquareController->addDefaultComponents();
        
        m_pContext->getGameObjectManager()->addComponent(hSquareController);
        
        // add the same scene node to square controller
        Array<int> alllowedEventsToPropagate(*m_pContext, m_arena); // we will pass empty array as allowed events to propagate so that when we add
        // scene node to the square controller, the square controller doesnt try to handle scene node's events
        // because scene node handles events trhough scene graph, and is child of sqaure controller just for referencing purposes
        pSquareController->addComponent(hSN, &alllowedEventsToPropagate);
    }
	// manually add level
	// note level loading is done completely through lua, there is no C++ function that lua wraps around
	// so we need to call lua code through C++

	// the Basic level contains one light source with 100% ambient, that is all objects are shown completely lit

	pLuaEnv->runString("LevelLoader.loadLevel('basic.x_level.levela', 'Basic')");
/*
Note: here's how you would hardcode the creation of light:
	//Default Lights
	Handle hLight1("LIGHT", sizeof(Light));
	Light *pLight1 = new(hLight1) Light(
		hLight1,
		Vector3(-100.0f, 260.0f, 0.0f), //Position
		Vector3(0.0f, 0.0f, 1.0f), //Direction
		Vector4(1.0f, 1.0f, 1.0f, 1.0f), //Ambient
		Vector4(0.0f, 0.0f, 0.0f, 1.0f), //Diffuse
		Vector4(0.0f, 0.0f, 0.0f, 1.0f), //Specular
		Vector3(0.000005f, 0.000005f, 0.000005f), //Attenuation (x, y, z)
		1.0f, //Spot power
		90000.0f, //Range
		0 //Type = point
	);
	pLight1->addDefaultComponents();
	proot->m_lights.add(hLight1);
	proot->addComponent(hLight1);
*/
	return 1; // 1 (true) = success. no errors. TODO: add error checking
}

}
}
