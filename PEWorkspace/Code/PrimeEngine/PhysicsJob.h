#pragma once

#include "PrimeEngineIncludes.h"

namespace PE {
    void physicsThreadFunctionJob(void* params);
    void runPhysicsSingleFrameThreaded(PE::GameContext& ctx);
    void runPhysicsThreadSingleFrameThreaded(PE::GameContext& ctx);
}