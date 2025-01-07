
#include "TextSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "TextMesh.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "MeshManager.h"
#include "MeshInstance.h"

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(TextSceneNode, SceneNode);


// Constructor -------------------------------------------------------------
TextSceneNode::TextSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: SceneNode(context, arena, hMyself)
{
	m_cachedAspectRatio = 1.0f;
	m_scale = 1.0f;
	if (IRenderer* pS = context.getGPUScreen())
		m_cachedAspectRatio = float(pS->getWidth()) / float(pS->getHeight());
}

void TextSceneNode::addDefaultComponents()
{
	SceneNode::addDefaultComponents();
	
	
	// event handlers
	PE_REGISTER_EVENT_HANDLER(Events::Event_PRE_GATHER_DRAWCALLS, TextSceneNode::do_PRE_GATHER_DRAWCALLS);
}

void TextSceneNode::setSelfAndMeshAssetEnabled(bool enabled)
{
	setEnabled(enabled);

	if (m_hMyTextMesh.isValid())
	{
		m_hMyTextMesh.getObject<Component>()->setEnabled(enabled);
	}
}


void TextSceneNode::loadFromString_needsRC(const char *str, DrawType drawType, int &threadOwnershipMask)
{
	m_drawType = drawType;

	TextMesh *pTextMesh = NULL;
	if (m_hMyTextMesh.isValid())
	{
		 pTextMesh = m_hMyTextMesh.getObject<TextMesh>();
	}
	else
	{
		m_hMyTextMesh = PE::Handle("TEXTMESH", sizeof(TextMesh));
		pTextMesh = new(m_hMyTextMesh) TextMesh(*m_pContext, m_arena, m_hMyTextMesh);
		pTextMesh->addDefaultComponents();

		m_pContext->getMeshManager()->registerAsset(m_hMyTextMesh);

		m_hMyTextMeshInstance = PE::Handle("MeshInstance", sizeof(MeshInstance));
		MeshInstance *pInstance = new(m_hMyTextMeshInstance) MeshInstance(*m_pContext, m_arena, m_hMyTextMeshInstance);
		pInstance->addDefaultComponents();
		pInstance->initFromRegisteredAsset(m_hMyTextMesh);

		
		addComponent(m_hMyTextMeshInstance);
	}

    PE::IRenderer::checkForErrors("");

	const char *tech = 0;
	if (drawType == Overlay2D_3DPos || drawType == Overlay2D)
		tech = "StdMesh_2D_Diffuse_A_RGBIntensity_Tech";
	if (drawType == InWorldFacingCamera)
		tech = "StdMesh_Diffuse_Tech";

	pTextMesh->loadFromString_needsRC(str, tech, threadOwnershipMask);
}

void TextSceneNode::do_PRE_GATHER_DRAWCALLS(Events::Event *pEvt)
{
	Events::Event_PRE_GATHER_DRAWCALLS *pDrawEvent = NULL;
	pDrawEvent = (Events::Event_PRE_GATHER_DRAWCALLS *)(pEvt);

	Matrix4x4 projectionViewWorldMatrix = pDrawEvent->m_projectionViewTransform;
	Matrix4x4 worldMatrix;

	if (!m_hMyTextMeshInstance.isValid())
		return;
	
	TextMesh *pTextMesh = m_hMyTextMesh.getObject<TextMesh>();
	
	if (m_drawType == InWorldFacingCamera)
	{
		m_worldTransform.turnTo(pDrawEvent->m_eyePos);
	}

	float numCharsInFullLine = 100.0f;
	numCharsInFullLine *= .5; // need to divide by 2.0 since screen goes from -1 to 1, not 0..1

	if (m_drawType == Overlay2D_3DPos)
	{
		worldMatrix = m_worldTransform;

		projectionViewWorldMatrix = projectionViewWorldMatrix * worldMatrix;

		Vector3 pos(0,0,0);
		pos = projectionViewWorldMatrix * pos;

		if (pos.m_x < -1.0f || pos.m_x > 1.0f || pos.m_z <= 0.0f || pos.m_z > 1.0f)
		{
			// this will cancel further event handling by this mesh only
			pEvt->m_cancelSiblingAndChildEventHandling = true;
			return;
		}

		float factor = 1.0f / (numCharsInFullLine);// * m_scale;
		Matrix4x4 scale;
		scale.importScale(factor, factor*m_cachedAspectRatio, 1.f);
		
		m_worldTransform.loadIdentity();
		m_worldTransform.setPos(Vector3(pos.m_x - factor * pTextMesh->m_textLength * .5f, pos.m_y, 0));
		m_worldTransform = m_worldTransform * scale;
	}
	if (m_drawType == Overlay2D)
	{
		worldMatrix = m_worldTransform;

		float factor = 1.0f / (numCharsInFullLine) * m_scale;

		Matrix4x4 scale;
		scale.importScale(factor, factor*m_cachedAspectRatio, 1.f);
		m_worldTransform = worldMatrix * scale;
	}


}

}; // namespace Components
}; // namespace PE
