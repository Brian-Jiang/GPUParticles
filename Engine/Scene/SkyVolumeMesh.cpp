#include "SkyVolumeMesh.h"

namespace PE {
namespace Components {

SkyVolume::SkyVolume(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, int &threadOwnershipMask) : Mesh(context, arena, hMyself)
{
	MeshCPU m(context, arena);
	m.ReadMesh("skyvolumemesh_mesh.mesha", "Default", "");
	loadFromMeshCPU_needsRC(m, threadOwnershipMask);
}

}; // namespace Components
}; // namespace PE
