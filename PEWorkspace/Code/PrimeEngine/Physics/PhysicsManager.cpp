#include "PhysicsManager.h"

#include <algorithm>
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Math/Matrix4x4.h"

namespace PE {
    namespace Components {
        Handle PhysicsManager::s_hInstance;

        PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

        PhysicsManager::PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
            : Component(context, arena, hMyself), m_nodes(context, arena, 32)
        {
            m_components.reset(32);
        }

        void PhysicsManager::Construct(PE::GameContext& context, PE::MemoryArena arena) {
            Handle h("ROOT_PHYSICS_MANAGER", sizeof(PhysicsManager));
            PhysicsManager* pPhysicsManager = new(h) PhysicsManager(context, arena, h);
            pPhysicsManager->addDefaultComponents();
            SetInstance(h);
        }

        void PhysicsManager::addDefaultComponents() {
            Component::addDefaultComponents();

            PE_REGISTER_EVENT_HANDLER(Events::Event_Update_Physics, PhysicsManager::do_Update_Physics);
        }

        void PhysicsManager::AddNode(Handle hNode) {
            m_nodes.add(hNode);
        }

        void PhysicsManager::do_Update_Physics(Events::Event* pEvt) {
            auto nodeCount = m_nodes.m_size;
            for (int i = 0; i < nodeCount; i++) {
                auto node = m_nodes[i].getObject<PhysicsNode>();
                auto nodeType = node->m_type;
                if (nodeType == PNUndefined || node->m_isStatic || nodeType == PNBox) {
                    continue;
                }

                auto delta = node->m_targetPosition - node->m_currentPosition;
                bool verticleCollision = false;
                Vector3 outDelta;
                if (nodeType == PNSphere) {
                    auto sphere = node->m_boundingSphere;
                    for (int i = 0; i < nodeCount; i++) {
                        auto nodeOther = m_nodes[i].getObject<PhysicsNode>();
                        if (CalculateNodeCollision(node, nodeOther, delta, outDelta)) {
                            delta = outDelta;
                        }

                        if (!verticleCollision && DetectGroundCollision(node, nodeOther)) {
                            verticleCollision = true;
                        }
                    }
                }
                else if (nodeType == PNBox) {
                    auto box = node->m_boundingBox;
                    //if (node->m_targetPosition != box.m_center) {
                    //    box.m_center = node->m_targetPosition;
                    //}
                }

                if (!verticleCollision) {
                    delta.m_y -= 0.1;
                }

                node->FinalizePosition(node->m_currentPosition + delta);
            }
        }

        bool PhysicsManager::CalculateNodeCollision(PhysicsNode* node1, PhysicsNode* node2, const Vector3& delta, Vector3& outDelta) {
            if (node1->m_type == PNBox && node2->m_type == PNBox || node1->m_type == PNSphere && node2->m_type == PNSphere) {
                return false;
            }

            Vector3 boxCenter, sphereCenter, boxHalfSize, boxDirection1, boxDirection2, boxDirection3;
            float radius;
            if (node1->m_type == PNBox && node2->m_type == PNSphere) {
                return false;
                //std::swap(node1, node2);
            }  // left: sphere, right: box

            if (node1->m_type == PNSphere && node2->m_type == PNBox) {
                auto& sphere = node1->m_boundingSphere;
                auto& box = node2->m_boundingBox;
                auto boxTransform = node2->m_transform;
                boxTransform.normalizeUVN();

                boxCenter = boxTransform * box.m_center;
                boxHalfSize = box.m_extent / 2.0;
                if (boxHalfSize.m_y == 0.0) {
                    boxHalfSize.m_y = 0.1;
                }
                boxDirection1 = boxTransform.getU();
                boxDirection2 = boxTransform.getV();
                boxDirection3 = boxTransform.getN();

                auto sphereTransform = node1->m_transform;
                sphereCenter = sphereTransform * sphere.m_center;
                radius = sphere.m_radius;
            }
            else {
                return false;
            }

            Vector3 boxToSphere = sphereCenter - boxCenter;
            auto dist1 = boxToSphere.dotProduct(boxDirection1);
            dist1 = std::max(-boxHalfSize.m_x, std::min(boxHalfSize.m_x, dist1));
            auto dist2 = boxToSphere.dotProduct(boxDirection2);
            dist2 = std::max(-boxHalfSize.m_y, std::min(boxHalfSize.m_y, dist2));
            auto dist3 = boxToSphere.dotProduct(boxDirection3);
            dist3 = std::max(-boxHalfSize.m_z, std::min(boxHalfSize.m_z, dist3));

            Vector3 boxClosestPoint = boxCenter + dist1 * boxDirection1 + dist2 * boxDirection2 + dist3 * boxDirection3;
            Vector3 closestPointToSphere = boxClosestPoint - sphereCenter;
            auto distanceSqr = closestPointToSphere.lengthSqr();
            auto hasCollision = distanceSqr <= radius * radius;
            if (distanceSqr == 0.0) {
                return false;
            }
            if (hasCollision) {
                Vector3 contactNormal(closestPointToSphere);
                contactNormal.normalize();
                if (contactNormal.dotProduct(delta) < 0.0) {
                    outDelta = delta;
                }
                else {
                    auto inDelta = contactNormal.dotProduct(delta) * contactNormal;
                    outDelta = delta - inDelta;
                }
                
            }

            return hasCollision;
        }

        bool PhysicsManager::DetectGroundCollision(PhysicsNode* node1, PhysicsNode* node2) {
            if (node1->m_type == PNBox && node2->m_type == PNBox || node1->m_type == PNSphere && node2->m_type == PNSphere) {
                return false;
            }

            Vector3 boxCenter, sphereCenter, boxHalfSize, boxDirection1, boxDirection2, boxDirection3;
            float radius;
            if (node1->m_type == PNBox && node2->m_type == PNSphere) {
                return false;
                //std::swap(node1, node2);
            }  // left: sphere, right: box

            if (node1->m_type == PNSphere && node2->m_type == PNBox) {
                auto& sphere = node1->m_boundingSphere;
                auto& box = node2->m_boundingBox;
                auto boxTransform = node2->m_transform;
                boxTransform.normalizeUVN();

                boxCenter = boxTransform * box.m_center;
                boxHalfSize = box.m_extent / 2.0;
                if (boxHalfSize.m_y == 0.0) {
                    boxHalfSize.m_y = 0.1;
                }
                boxDirection1 = boxTransform.getU();
                boxDirection2 = boxTransform.getV();
                boxDirection3 = boxTransform.getN();

                auto sphereTransform = node1->m_transform;
                sphereCenter = sphereTransform * sphere.m_center;
                radius = sphere.m_radius;
            }
            else {
                return false;
            }

            Vector3 boxToSphere = sphereCenter - boxCenter;
            Vector3 origin(boxToSphere.dotProduct(boxDirection1), boxToSphere.dotProduct(boxDirection2), boxToSphere.dotProduct(boxDirection3));
            Vector3 worldDirection(0.0, -1.0, 0.0);
            Vector3 direction(worldDirection.dotProduct(boxDirection1), worldDirection.dotProduct(boxDirection2), worldDirection.dotProduct(boxDirection3));
            Vector3 low(-boxHalfSize.m_x, -boxHalfSize.m_y, -boxHalfSize.m_z);
            Vector3 high(boxHalfSize.m_x, boxHalfSize.m_y, boxHalfSize.m_z);
            Vector3 tMin = (low - origin) / direction;
            Vector3 tMax = (high - origin) / direction;
            if (direction.m_x < 0.0) {
                std::swap(tMin.m_x, tMax.m_x);
            }

            if (direction.m_y < 0.0) {
                std::swap(tMin.m_y, tMax.m_y);
            }

            if (direction.m_z < 0.0) {
                std::swap(tMin.m_z, tMax.m_z);
            }

            auto t1 = std::max(std::max(tMin.m_x, tMin.m_y), tMin.m_z);
            auto t2 = std::min(std::min(tMax.m_x, tMax.m_y), tMax.m_z);
            auto hasCollision = t1 <= t2 && t2 > 0.0;
            return hasCollision;
        }
    }
}