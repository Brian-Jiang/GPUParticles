#include "PrimeEngine/PrimeEngineIncludes.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Physics/PhysicsManager.h"
#include "PrimeEngine/Scene/ParticleEmitter.h"
#include "GlobalRegistry.h"


namespace PE {

bool setLuaMetaDataOnly = 0;

void Register(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry)
{
	pLuaEnv->StartRootRegistrationTable();
	// start root
	{
		pLuaEnv->StartRegistrationTable("PE");
		// start root.PE
		{
			pLuaEnv->StartRegistrationTable("Components");
			// start root.PE.Components
			{
				PE::Components::Component::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::LuaEnvironment::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::ClientLuaEnvironment::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::ServerLuaEnvironment::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::GameObjectManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::NetworkManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::ClientNetworkManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::ServerNetworkManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::ConnectionManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::ClientConnectionManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::ServerConnectionManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::Camera::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::StreamManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::EventManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Input::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::DefaultGameControls::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::DrawList::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::CameraManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Log::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Light::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::SceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::RootSceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::CameraSceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::JointSceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::TextSceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
						PE::Components::InstancingSceneNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::MeshManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Mesh::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::MeshInstance::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::ParticleEmitter::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Skeleton::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::SkeletonInstance::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::TextMesh::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::LineMesh::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::DebugRenderer::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::DefaultAnimationSM::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::SingleHandler_DRAW::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::PESSEH_CHANGE_TO_DEBUG_SHADER::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::PESSEH_POP_SHADERS::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::PESSEH_DRAW_Z_ONLY::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::Effect::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::PhysicsManager::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Components::PhysicsNode::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
                    #if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11
                        PE::Components::DX9_XInput::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					#elif PE_PLAT_IS_PS3 || PE_PLAT_IS_PSVITA
						PE::Components::PS3_PadInput::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					#else
						pRegistry->getNextClassId(); // advance counter so that class ids don't differ between platforms
                    #endif
                    #if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | APIABSTRACTION_GLPC
                        PE::Components::DX9_KeyboardMouse::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					#else
						pRegistry->getNextClassId(); // advance counter so that class ids don't differ between platforms
					#endif
			}
			// end root.PE.Components
			pLuaEnv->EndRegistrationTable();


			pLuaEnv->StartRegistrationTable("Events");
			// start root.PE.Events
			{
				PE::Events::Event::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ADDED_AS_COMPONENT::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_UPDATE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_SCENE_GRAPH_UPDATE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PRE_RENDER_needsRC::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_GATHER_DRAWCALLS::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_GATHER_DRAWCALLS_Z_ONLY::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CALCULATE_TRANSFORMATIONS::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PRE_GATHER_DRAWCALLS::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_POP_SHADERS::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CHANGE_TO_DEBUG_SHADER::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CLOSED_WINDOW::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ANIMATION_ENDED::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PLAY_ANIMATION::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_IK_REPORT::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CREATE_LIGHT::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CREATE_MESH::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CREATE_SKELETON::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CREATE_ANIM_SET::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CREATE_NAVGRID::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PHYSICS_END::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PHYSICS_START::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_VORTEX_ENDED::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_MOVE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_SET_DEBUG_TARGET_HANDLE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CONSTRUCT_SOUND::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ADDED_AS_COMPONENT::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_CHARACTER_HIT_BY_MELEE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_Update_Physics::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					#if APIABSTRACTION_IOS
						// IOS Input Events
						PE::Events::Event_IOS_TOUCH_MOVED::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					#else
						pRegistry->getNextClassId(); // advance counter so that class ids don't differ between platforms
					#endif

					// Button down events
					PE::Events::Event_BUTTON_Y_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_B_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_A_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_X_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_THUMB_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_THUMB_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_SHOULDER_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_SHOULDER_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_BACK_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_START_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_N_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_S_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_E_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_W_DOWN::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					// Button up events
					PE::Events::Event_BUTTON_Y_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_B_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_A_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_X_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_THUMB_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_THUMB_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_SHOULDER_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_SHOULDER_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_BACK_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_START_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_N_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_S_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_E_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_W_UP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					// Thumb & trigger move events
					PE::Events::Event_ANALOG_L_THUMB_MOVE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ANALOG_R_THUMB_MOVE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ANALOG_L_TRIGGER_MOVE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ANALOG_R_TRIGGER_MOVE::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					// Button held events
					PE::Events::Event_BUTTON_Y_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_B_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_A_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_X_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_THUMB_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_THUMB_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_L_SHOULDER_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_R_SHOULDER_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_BACK_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_BUTTON_START_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_N_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_S_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_E_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_PAD_W_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);


					// Game Events
					PE::Events::Event_FLY_CAMERA::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_ROTATE_CAMERA::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					// Standard keyboard events
					PE::Events::Event_KEY_A_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_S_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_D_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_W_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_PERIOD_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_COMMA_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_K_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_L_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_LEFT_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_DOWN_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_RIGHT_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					PE::Events::Event_KEY_UP_HELD::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

					PE::Events::Event_SERVER_CLIENT_CONNECTION_ACK::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);

			}
			// end root.PE.Events
			pLuaEnv->EndRegistrationTable();
		}
		// end root.PE
		pLuaEnv->EndRegistrationTable();
	}
	// end root
	pLuaEnv->EndRootRegistrationTable();

	setLuaMetaDataOnly = true; // make sure on next pass we dont reset class id, we just set registration values in lua
}
};
