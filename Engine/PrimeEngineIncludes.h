
//akovalovs: by including this file user can use any PrimeEngine classes
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#endif

#include "PrimeEngine/MemoryManagement/MemoryManager.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/APIAbstraction/Timer/Timer.h"

#include "PrimeEngine/Utils/PEClassDecl.h"

//todo: include all these for IOS too


#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "PrimeEngine/Geometry/IndexBufferCPU/IndexBufferCPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkinWeightsCPU.h"
#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetGPUManager.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPUManager.h"
#include "PrimeEngine/Geometry/NormalBufferCPU/NormalBufferCPUManager.h"
#include "PrimeEngine/Geometry/TexCoordBufferCPU/TexCoordBufferCPUManager.h"
#include "PrimeEngine/APIAbstraction/Texture/SamplerState.h"

#include "Game/Common/GameContext.h"
#include "GameObjectModel/GameObjectManager.h"
#include "Lua/LuaEnvironment.h"

#include "PrimeEngine/Lua/Client/ClientLuaEnvironment.h"
#include "PrimeEngine/Lua/Server/ServerLuaEnvironment.h"

#include "Lua/EventGlue/EventDataCreators.h"
#include "Networking/NetworkManager.h"
#include "Networking/Client/ClientNetworkManager.h"
#include "Networking/Server/ServerNetworkManager.h"

#include "Networking/ConnectionManager.h"
#include "Networking/Client/ClientConnectionManager.h"
#include "Networking/Server/ServerConnectionManager.h"
#include "Networking/StreamManager.h"
#include "Networking/EventManager.h"


#include "PrimeEngine/APIAbstraction/Input/Input.h"

#include "PrimeEngine/Geometry/MeshCPU/MeshCPU.h"
#include "PrimeEngine/Geometry/MaterialCPU/MaterialCPU.h"
#include "PrimeEngine/Geometry/MaterialCPU/MaterialSetCPU.h"
#include "PrimeEngine/APIAbstraction/DirectX9/DX9_XInput/DX9_XInput.h"
#include "PrimeEngine/APIAbstraction/DirectX9/DX9_KeyboardMouse/DX9_KeyboardMouse.h"
#include "PrimeEngine/Events/Event.h"

#include "Logging/Log.h"
#include "Profiling/Profiling.h"
#include "Game/Common/GlobalRegistry.h"


#include "PrimeEngine/Events/EventQueue.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Events/StandardControllerEvents.h"

#if APIABSTRACTION_IOS
#include "PrimeEngine/Events/StandardIOSEvents.h"
#endif

#include "PrimeEngine/Events/StandardEvents.h"

#include "Scene/SceneNode.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/Math/CameraOps.h"
#include "PrimeEngine/APIAbstraction/Texture/GPUTextureManager.h"
#include "PrimeEngine/Utils/StrToHandleMap.h"
#include "PrimeEngine/Utils/Dir.h"
#include "Scene/Mesh.h"
#include "Scene/MeshManager.h"
#include "Scene/MeshInstance.h"
#include "Scene/TextMesh.h"
#include "Scene/LineMesh.h"
#include "Scene/DefaultAnimationSM.h"
#include "Scene/RootSceneNode.h"
#include "Scene/CameraSceneNode.h"
#include "Scene/TextSceneNode.h"
#include "Scene/InstancingSceneNode.h"
#include "Scene/Light.h"
#include "Scene/CameraManager.h"
#include "Scene/DebugRenderer.h"

#include "GameObjectModel/Camera.h"
#include "GameObjectModel/DefaultGameControls/DefaultGameControls.h"



#include "Scene/DrawList.h"

#include "Sound/SoundManager.h"

#include "Logging/Log.h"

#include "Scene/SH_DRAW.h"


#include "Game/Client/ClientGame.h"
#include "Game/Server/ServerGame.h"

