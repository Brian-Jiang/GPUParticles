#include "PrimeEngine/PhysicsJob.h"
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Physics/PhysicsManager.h"

namespace PE {

    void PE::physicsThreadFunctionJob(void* params) {
        GameContext* pContext = static_cast<PE::GameContext*>(params);

        g_physicsThreadInitializationLock.lock();
        g_physicsThreadInitialized = true;
        g_physicsThreadExited = false;
        g_physicsThreadInitializationLock.unlock();

        g_physicsThreadLock.lock();
        g_physicsThreadInitializedCV.signal();

        while (1) {
            runPhysicsSingleFrameThreaded(*pContext);
            if (g_physicsThreadExited)
                return;
        }
        return;
    }

    void PE::runPhysicsSingleFrameThreaded(PE::GameContext& ctx) {
        while (!g_physicsThreadCanStart && !g_physicsThreadShouldExit) {
            bool success = g_physicsCanStartCV.sleep();
            assert(success);
        }
        g_physicsThreadCanStart = false; // set to false for next frame

        if (g_physicsThreadShouldExit) {
            //right now game thread is waiting on this thread to finish
            g_physicsThreadLock.unlock();
            g_physicsThreadExited = true;
            return;
        }

        runPhysicsThreadSingleFrameThreaded(ctx);
    }

    void PE::runPhysicsThreadSingleFrameThreaded(PE::GameContext& ctx) {
        Components::PhysicsManager::Instance()->do_Update_Physics(nullptr);
    }

}